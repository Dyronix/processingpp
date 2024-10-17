#include "render/render_batch_renderer.h"
#include "render/render_item_components.h"

#include "resources/shader_pool.h"

#include "util/log.h"
#include "util/color_ops.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static constexpr s32 _max_points = 9'000;
            static constexpr s32 _max_lines = 9'000;
            static constexpr s32 _max_triangles = 9'000;

            static constexpr s32 _max_texture_units = 8;

            //-------------------------------------------------------------------------
            static constexpr s32 _solid = 1 << 1;
            static constexpr s32 _wireframe = 1 << 0;

            static f32 _wireframe_linewidth = 3.0f;
            static s32 _wireframe_linecolor = 0x000000FF;

            //-------------------------------------------------------------------------
            void check_drawing_type(const batch* batch, GLenum type)
            {
                auto index_count = batch->active_index_count();
                switch (type)
                {
                case GL_LINES:
                    if (index_count % 2 != 0)
                    {
                        log::error("Trying to render invalid number of lines: {}", batch->active_index_count());
                        return;
                    }
                    break;
                case GL_TRIANGLES:
                    if (index_count % 3 != 0)
                    {
                        log::error("Trying to render invalid number of triangles: {}", batch->active_index_count());
                        return;
                    }
                    break;
                }
            }

            //-------------------------------------------------------------------------
            u32 topology(topology_type type)
            {
                switch (type)
                {
                case topology_type::POINTS: return GL_POINTS;
                case topology_type::LINES: return GL_LINES;
                case topology_type::TRIANGLES: return GL_TRIANGLES;
                }

                log::error("Invalid topology_type type specified, using GL_TRIANGLES");
                return GL_TRIANGLES;
            }
            //-------------------------------------------------------------------------
            u32 index_type()
            {
                if (sizeof(index) == sizeof(u32)) return GL_UNSIGNED_INT;
                if (sizeof(index) == sizeof(u16)) return GL_UNSIGNED_SHORT;

                log::error("Invalid index type specified: {}, using UNSIGNED_INT", typeid(index).name());
                return GL_UNSIGNED_INT;
            }

            //-------------------------------------------------------------------------
            u32 max_vertices(topology_type type)
            {
                s32 max_vertex_elements = 0;
                glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertex_elements);

                s32 max_vertices = 0;
                switch (type)
                {
                case topology_type::POINTS:
                    max_vertices = std::min(max_vertex_elements, _max_points);
                    break;
                case topology_type::LINES:
                    max_vertices = std::min(max_vertex_elements, _max_lines);
                    break;
                case topology_type::TRIANGLES:
                    max_vertices = std::min(max_vertex_elements, _max_triangles);
                    break;
                }

                return max_vertices;
            }
            //-------------------------------------------------------------------------
            u32 max_indices(topology_type type)
            {
                s32 max_index_elements = 0;
                glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_index_elements);

                s32 max_indices = 0;
                switch (type)
                {
                case topology_type::POINTS:
                    max_indices = std::min(max_index_elements, _max_points * 1);
                    break;
                case topology_type::LINES:
                    max_indices = std::min(max_index_elements, _max_lines * 2);
                    break;
                case topology_type::TRIANGLES:
                    max_indices = std::min(max_index_elements, _max_triangles * 3);
                    break;
                }

                return max_indices;
            }
            //-------------------------------------------------------------------------
            u32 max_textures()
            {
                s32 max_texture_units = 0;
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

                return std::min(max_texture_units, internal::_max_texture_units);
            }
        }

        // Batch Renderer
        //-------------------------------------------------------------------------
        void batch_renderer::set_wireframe_linewidth(f32 width) 
        {
            internal::_wireframe_linewidth = width; 
        }

        //-------------------------------------------------------------------------
        void batch_renderer::set_wireframe_linecolor(s32 color) 
        {
            internal::_wireframe_linecolor = color;
        }

        //-------------------------------------------------------------------------
        batch_renderer::batch_renderer(vertex_attribute_layout* layouts, u64 layout_count, std::string_view shader_tag, bool enable_texture_support)
            : m_shader_tag(shader_tag)
            , m_drawing_data_map()
            , m_rasterization_mode(internal::_solid)
            , m_layouts(layouts)
            , m_layout_count(layout_count)
            , m_texture_support(enable_texture_support)
        {}

        //-------------------------------------------------------------------------
        batch_renderer::~batch_renderer() = default;

        //-------------------------------------------------------------------------
        void batch_renderer::begin()
        {
            for (auto& pair : m_drawing_data_map)
            {
                pair.second.reset();
            }
        }

        //-------------------------------------------------------------------------
        void batch_renderer::render(const glm::mat4& vp)
        {
            glUseProgram(shader_program());
            u32 u_mpv_loc = glGetUniformLocation(shader_program(), "u_worldviewproj");
            glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

            for (auto& pair : m_drawing_data_map)
            {
                // No drawing data, early out
                if (!pair.second.has_drawing_data())
                {
                    return;
                }

                if (solid_rendering_supported())
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glUniform1i(glGetUniformLocation(shader_program(), "u_wireframe"), GL_FALSE);

                    on_render(pair.first, pair.second);
                }

                if (wireframe_rendering_supported())
                {
                    pair.second.load_first_batch();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glLineWidth(internal::_wireframe_linewidth);
                    glUniform1i(glGetUniformLocation(shader_program(), "u_wireframe"), GL_TRUE);
                    glm::vec4 wireframe_color = color::convert_color(internal::_wireframe_linecolor);
                    glUniform4fv(glGetUniformLocation(shader_program(), "u_wireframe_color"), 1, value_ptr(wireframe_color));

                    on_render(pair.first, pair.second);
                }
            }

            glUseProgram(0);
        }

        //-------------------------------------------------------------------------
        void batch_renderer::end()
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void batch_renderer::terminate()
        {
            for (auto& pair : m_drawing_data_map)
            {
                pair.second.release();
            }
        }

        void batch_renderer::append_drawing_data(topology_type topology, const render_item& item, const glm::vec4& color, const glm::mat4& world)
        {
            if (m_drawing_data_map.find(topology) == std::cend(m_drawing_data_map))
            {
                s32 max_vertices = internal::max_vertices(topology);
                s32 max_indices = internal::max_indices(topology);
                s32 max_textures = m_texture_support ? internal::max_textures() : -1;

                m_drawing_data_map.emplace(topology, batch_drawing_data(max_vertices, max_indices, max_textures, m_layouts, m_layout_count));
            }
            else
            {
                m_drawing_data_map.at(topology).append(item, color, world);
            }
        }

        //-------------------------------------------------------------------------
        void batch_renderer::enable_solid_rendering(bool enable)
        {
            if (enable)
            {
                m_rasterization_mode |= internal::_solid;
            }
            else
            {
                m_rasterization_mode &= ~internal::_solid;
            }
        }

        //-------------------------------------------------------------------------
        void batch_renderer::enable_wireframe_rendering(bool enable)
        {
            if (enable)
            {
                m_rasterization_mode |= internal::_wireframe;
            }
            else
            {
                m_rasterization_mode &= ~internal::_wireframe;
            }
        }

        //-------------------------------------------------------------------------
        bool batch_renderer::solid_rendering_supported() const 
        {
            return m_rasterization_mode & internal::_solid;
        }

        //-------------------------------------------------------------------------
        bool batch_renderer::wireframe_rendering_supported() const 
        {
            return m_rasterization_mode & internal::_wireframe;
        }

        //-------------------------------------------------------------------------
        u32 batch_renderer::shader_program() const 
        {
            return shader_pool::get_shader_program(m_shader_tag); 
        }

        // Primitive Batch Renderer
        //-------------------------------------------------------------------------
        primitive_batch_renderer::primitive_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag)
            :batch_renderer(layouts, layout_cout, shader_tag, false)
        {

        }

        //-------------------------------------------------------------------------
        primitive_batch_renderer::~primitive_batch_renderer() = default;

        //-------------------------------------------------------------------------
        void primitive_batch_renderer::on_render(topology_type topology, batch_drawing_data& drawing_data)
        {
            GLenum gl_topology = internal::topology(topology);

            auto batch = drawing_data.next_batch();
            if (batch != nullptr)
            {
                glBindVertexArray(drawing_data.vao());
                glBindBuffer(GL_ARRAY_BUFFER, drawing_data.vbo());

                while (batch != nullptr)
                {
                    #ifndef NDEBUG
                    internal::check_drawing_type(batch, gl_topology);
                    #endif

                    glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices());
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices());

                    glDrawElements(gl_topology, batch->active_index_count(), internal::index_type(), nullptr);

                    batch = drawing_data.next_batch();
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

        // Texture Batch Renderer
        //-------------------------------------------------------------------------
        texture_batch_renderer::texture_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag)
            :batch_renderer(layouts, layout_cout, shader_tag, true)
        {

        }

        //-------------------------------------------------------------------------
        texture_batch_renderer::~texture_batch_renderer() = default;

        //-------------------------------------------------------------------------
        void texture_batch_renderer::on_render(topology_type topology, batch_drawing_data& drawing_data)
        {
            GLenum gl_topology = internal::topology(topology);

            auto batch = drawing_data.next_batch();
            if (batch != nullptr)
            {
                glBindVertexArray(drawing_data.vao());
                glBindBuffer(GL_ARRAY_BUFFER, drawing_data.vbo());

                while (batch != nullptr)
                {
                    assert(batch->samplers());
                    assert(batch->textures());

                    u32 u_tex_samplers_loc = glGetUniformLocation(shader_program(), "s_image");
                    glUniform1iv(u_tex_samplers_loc, batch->active_sampler_count(), batch->samplers());

                    #ifndef NDEBUG
                    internal::check_drawing_type(batch, gl_topology);
                    #endif

                    s32 i = 0;
                    s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                    for (int i = 0; i < batch->active_texture_count(); ++i)
                    {
                        glActiveTexture(GL_TEXTURE0 + (offset * i));
                        glBindTexture(GL_TEXTURE_2D, batch->textures()[i]);
                    }

                    glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices());
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices());

                    glDrawElements(gl_topology, batch->active_index_count(), internal::index_type(), nullptr);

                    batch = drawing_data.next_batch();
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }
    }
}