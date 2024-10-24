#include "render/render_batch_renderer.h"
#include "render/render_item_components.h"
#include "render/render_shaders.h"

#include "render/opengl/render_gl_error.h"

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
                GL_CALL(glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertex_elements));

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
                GL_CALL(glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_index_elements));

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
                GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units));

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
        batch_renderer::batch_renderer(vertex_attribute_layout* layouts, u64 layout_count, const std::string& shader_tag, bool enable_texture_support)
            : m_shader_tag(shader_tag)
            , m_drawing_data_map()
            , m_rasterization_mode(internal::_solid)
            , m_layouts(layouts)
            , m_layout_count(layout_count)
            , m_texture_support(enable_texture_support)
            , m_batch_buffer_policy(batch_buffer_policy::IMMEDIATE)
            , m_batch_render_policy(batch_render_policy::BUILD_IN)
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
            if (!has_drawing_data())
            {
                // No drawing data, early out
                return;
            }

            GL_CALL(glUseProgram(shader_program()));

            shaders::push_uniform(shader_program(), "u_worldviewproj", vp);

            for (auto& pair : m_drawing_data_map)
            {
                // No drawing data, early out
                if (!pair.second.has_drawing_data())
                {
                    return;
                }

                // Build in render policy
                if (m_batch_render_policy == batch_render_policy::BUILD_IN)
                {
                    if (solid_rendering_supported())
                    {
                        GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

                        shaders::push_uniform(shader_program(), "u_wireframe", GL_FALSE);

                        on_render(pair.first, pair.second);
                    }

                    if (wireframe_rendering_supported())
                    {
                        pair.second.load_first_batch();

                        GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                        GL_CALL(glLineWidth(internal::_wireframe_linewidth));
                        
                        shaders::push_uniform(shader_program(), "u_wireframe", GL_TRUE);
                        shaders::push_uniform(shader_program(), "u_wireframe_color", color::convert_color(internal::_wireframe_linecolor));

                        on_render(pair.first, pair.second);
                    }
                }
                // User defined render policy
                else
                {
                    on_render(pair.first, pair.second);
                }
            }

            GL_CALL(glUseProgram(0));
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

                m_drawing_data_map.emplace(topology, batch_drawing_data(max_vertices, max_indices, max_textures, m_layouts, m_layout_count, m_batch_buffer_policy));
            }

            m_drawing_data_map.at(topology).append(item, color, world);
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
        void batch_renderer::buffer_policy(batch_buffer_policy buffer_policy)
        {
            m_batch_buffer_policy = buffer_policy;
        }

        //-------------------------------------------------------------------------
        void batch_renderer::render_policy(batch_render_policy render_policy)
        {
            m_batch_render_policy = render_policy;
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
        bool batch_renderer::has_drawing_data() const
        {
            return std::any_of(std::cbegin(m_drawing_data_map), std::cend(m_drawing_data_map),
                [](const auto& pair)
            {
                return pair.second.has_drawing_data();
            });
        }

        //-------------------------------------------------------------------------
        u32 batch_renderer::shader_program() const 
        {
            return shader_pool::get_shader_program(m_shader_tag); 
        }

        //-------------------------------------------------------------------------
        batch_buffer_policy batch_renderer::buffer_policy() const
        {
            return m_batch_buffer_policy;
        }

        //-------------------------------------------------------------------------
        batch_render_policy batch_renderer::render_policy() const
        {
            return m_batch_render_policy;
        }

        // Primitive Batch Renderer
        //-------------------------------------------------------------------------
        primitive_batch_renderer::primitive_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag)
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
                GL_CALL(glBindVertexArray(drawing_data.vao()));
                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, drawing_data.vbo()));

                while (batch != nullptr)
                {
                    #ifndef NDEBUG
                    internal::check_drawing_type(batch, gl_topology);
                    #endif

                    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices()));
                    GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices()));

                    shaders::apply_uniforms(shader_program());
                    GL_CALL(glDrawElements(gl_topology, batch->active_index_count(), internal::index_type(), nullptr));

                    batch = drawing_data.next_batch();
                }

                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
                GL_CALL(glBindVertexArray(0));
            }
        }

        // Texture Batch Renderer
        //-------------------------------------------------------------------------
        texture_batch_renderer::texture_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag)
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
                GL_CALL(glBindVertexArray(drawing_data.vao()));
                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, drawing_data.vbo()));

                while (batch != nullptr)
                {
                    assert(batch->samplers());
                    assert(batch->textures());

                    shaders::push_uniform_array(shader_program(), "s_image", batch->active_sampler_count(), batch->samplers());

                    #ifndef NDEBUG
                    internal::check_drawing_type(batch, gl_topology);
                    #endif

                    s32 i = 0;
                    s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                    for (int i = 0; i < batch->active_texture_count(); ++i)
                    {
                        GL_CALL(glActiveTexture(GL_TEXTURE0 + (offset * i)));
                        GL_CALL(glBindTexture(GL_TEXTURE_2D, batch->textures()[i]));
                    }

                    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices()));
                    GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices()));

                    shaders::apply_uniforms(shader_program());
                    GL_CALL(glDrawElements(gl_topology, batch->active_index_count(), internal::index_type(), nullptr));

                    batch = drawing_data.next_batch();
                }

                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
                GL_CALL(glBindVertexArray(0));
            }
        }
    }
}