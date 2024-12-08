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
            static void check_drawing_type(const batch* batch, GLenum type)
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
            static u32 topology(topology_type type)
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
            static u32 index_type()
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
            , m_buffer_policy(render_buffer_policy::IMMEDIATE)
            , m_render_policy(render_draw_policy::BUILD_IN)
        {
            draw_policy(m_render_policy);
        }

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

                for (auto& render_fn : m_render_fns)
                {
                    render_fn(pair.first, pair.second);
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

        //-------------------------------------------------------------------------
        void batch_renderer::append_drawing_data(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {
            if (m_drawing_data_map.find(topology) == std::cend(m_drawing_data_map))
            {
                s32 max_vertices = internal::max_vertices(topology);
                s32 max_indices = internal::max_indices(topology);
                s32 max_textures = m_texture_support ? internal::max_textures() : -1;

                m_drawing_data_map.emplace(topology, batch_drawing_data(max_vertices, max_indices, max_textures, m_layouts, m_layout_count, m_buffer_policy));
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

            // Make sure the solid_render function is either excluded or included in the render policy
            draw_policy(m_render_policy);
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

            // Make sure the wireframe_render function is either excluded or included in the render policy
            draw_policy(m_render_policy);
        }

        //-------------------------------------------------------------------------
        void batch_renderer::buffer_policy(render_buffer_policy render_buffer_policy)
        {
            m_buffer_policy = render_buffer_policy;
        }

        //-------------------------------------------------------------------------
        void batch_renderer::draw_policy(render_draw_policy render_draw_policy)
        {
            m_render_policy = render_draw_policy;
            m_render_fns.clear();

            switch (render_draw_policy)
            {
            case render_draw_policy::BUILD_IN:
                if (solid_rendering_supported())
                {
                    m_render_fns.push_back([&](topology_type topology, batch_drawing_data& drawing_data) {
                        solid_render(topology, drawing_data);
                    });
                }
                if (wireframe_rendering_supported())
                {
                    m_render_fns.push_back([&](topology_type topology, batch_drawing_data& drawing_data) {
                        wireframe_render(topology, drawing_data);
                    });
                }
                break;
            case render_draw_policy::CUSTOM:
                m_render_fns.push_back([&](topology_type topology, batch_drawing_data& drawing_data) {
                    on_render(topology, drawing_data);
                });
                break;
            }
        }

        //-------------------------------------------------------------------------
        void batch_renderer::user_shader_program(const std::string& tag)
        {
            if (shader_pool::has_shader(tag))
            {
                m_user_shader_tag = tag;
            }
        }

        //-------------------------------------------------------------------------
        void batch_renderer::reset_user_shader_program()
        {
            m_user_shader_tag = {};
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
            return shader_pool::get_shader_program(m_user_shader_tag.empty() ? m_shader_tag : m_user_shader_tag);
        }

        //-------------------------------------------------------------------------
        render_buffer_policy batch_renderer::buffer_policy() const
        {
            return m_buffer_policy;
        }

        //-------------------------------------------------------------------------
        render_draw_policy batch_renderer::draw_policy() const
        {
            return m_render_policy;
        }

        //-------------------------------------------------------------------------
        void batch_renderer::solid_render(topology_type topology, batch_drawing_data& drawing_data)
        {
            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

            shaders::push_uniform(shader_program(), "u_wireframe", GL_FALSE);

            on_render(topology, drawing_data);
        }

        //-------------------------------------------------------------------------
        void batch_renderer::wireframe_render(topology_type topology, batch_drawing_data& drawing_data)
        {
            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
            GL_CALL(glLineWidth(internal::_wireframe_linewidth));

            shaders::push_uniform(shader_program(), "u_wireframe", GL_TRUE);
            shaders::push_uniform(shader_program(), "u_wireframe_color", color::convert_color(internal::_wireframe_linecolor));

            on_render(topology, drawing_data);
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
            auto batch = drawing_data.first_batch();
            if (batch != nullptr)
            {
                while (batch != nullptr)
                {
                    batch->bind();
                    batch->submit();
                    batch->draw(topology, shader_program());

                    batch = drawing_data.next_batch();

                    // GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices()));

                    // if (batch->active_index_count() != 0)
                    // {
                    //     GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices()));

                    //     shaders::apply_uniforms(shader_program());
                    //     GL_CALL(glDrawElements(gl_topology, batch->active_index_count(), internal::index_type(), nullptr));
                    // }
                    // else
                    // {
                    //     shaders::apply_uniforms(shader_program());
                    //     GL_CALL(glDrawArrays(gl_topology, 0, batch->active_vertex_count()));
                    // }
                }
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
            auto batch = drawing_data.first_batch();
            if (batch != nullptr)
            {
                while (batch != nullptr)
                {
                    assert(batch->samplers());
                    assert(batch->textures());

                    batch->bind();

                    shaders::push_uniform_array(shader_program(), "s_image", batch->active_sampler_count(), batch->samplers());

                    s32 i = 0;
                    s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                    for (int i = 0; i < batch->active_texture_count(); ++i)
                    {
                        GL_CALL(glActiveTexture(GL_TEXTURE0 + (offset * i)));
                        GL_CALL(glBindTexture(GL_TEXTURE_2D, batch->textures()[i]));
                    }

                    batch->submit();
                    batch->draw(topology, shader_program());

                    batch = drawing_data.next_batch();

                    // GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices()));

                    // if (batch->active_index_count() != 0)
                    // {
                    //     GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices()));

                    //     shaders::apply_uniforms(shader_program());
                    //     GL_CALL(glDrawElements(gl_topology, batch->active_index_count(), internal::index_type(), nullptr));
                    // }
                    // else
                    // {
                    //     shaders::apply_uniforms(shader_program());
                    //     GL_CALL(glDrawArrays(gl_topology, 0, batch->active_vertex_count()));
                    // }
                }

                batch->unbind();
            }
        }
    }
}