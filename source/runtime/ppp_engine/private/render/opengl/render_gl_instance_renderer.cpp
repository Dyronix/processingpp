#include "render/render_instance_renderer.h"
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
            static constexpr s32 _solid = 1 << 1;
            static constexpr s32 _wireframe = 1 << 0;

            static f32 _wireframe_linewidth = 3.0f;
            static s32 _wireframe_linecolor = 0x000000FF;

            //-------------------------------------------------------------------------
            static void check_drawing_type(const irender_item* inst, GLenum type)
            {
                auto index_count = inst->index_count();
                switch (type)
                {
                case GL_LINES:
                    if (index_count % 2 != 0)
                    {
                        log::error("Trying to render invalid number of lines: {}", inst->index_count());
                        return;
                    }
                    break;
                case GL_TRIANGLES:
                    if (index_count % 3 != 0)
                    {
                        log::error("Trying to render invalid number of triangles: {}", inst->index_count());
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
        }

        // Batch Renderer
        //-------------------------------------------------------------------------
        void instance_renderer::set_wireframe_linewidth(f32 width)
        {
            internal::_wireframe_linewidth = width;
        }

        //-------------------------------------------------------------------------
        void instance_renderer::set_wireframe_linecolor(s32 color)
        {
            internal::_wireframe_linecolor = color;
        }

        //-------------------------------------------------------------------------
        instance_renderer::instance_renderer(vertex_attribute_layout* layouts, u64 layout_count, const std::string& shader_tag, bool enable_texture_support)
            : m_shader_tag(shader_tag)
            , m_instance_data_map()
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
        instance_renderer::~instance_renderer() = default;

        //-------------------------------------------------------------------------
        void instance_renderer::begin()
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void instance_renderer::render(const glm::mat4& vp)
        {
            if (!has_instance_data() || !has_drawing_data())
            {
                // No drawing data, early out
                return;
            }

            GL_CALL(glUseProgram(shader_program()));

            shaders::push_uniform(shader_program(), "u_worldviewproj", vp);

            for (auto& pair : m_instance_data_map)
            {
                // No drawing data, early out
                if (!pair.second.has_instance_data() || !pair.second.has_drawing_data())
                {
                    return;
                }

                for (auto& render_fn : m_render_fns)
                {
                    render_fn(pair.second);
                }
            }

            GL_CALL(glUseProgram(0));
        }

        //-------------------------------------------------------------------------
        void instance_renderer::end()
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void instance_renderer::terminate()
        {
            for (auto& pair : m_instance_data_map)
            {
                pair.second.release();
            }
        }

        //-------------------------------------------------------------------------
        void instance_renderer::append_instance(topology_type topology, const irender_item* instance)
        {
            if (m_instance_data_map.find(instance->id()) == std::cend(m_instance_data_map))
            {
                u64                     instance_id = instance->id();
                instance_drawing_data   instance_draw_data = instance_drawing_data(topology, instance, m_layouts, m_layout_count);

                m_instance_data_map.emplace(instance_id, std::move(instance_draw_data));
            }

            m_instance_data_map.at(instance->id()).increment_instance_count();
        }

        //-------------------------------------------------------------------------
        void instance_renderer::append_drawing_data(u64 geometry_id, const glm::vec4& color, const glm::mat4& world)
        {
            assert(m_instance_data_map.find(geometry_id) != std::cend(m_instance_data_map) && "No instance was found for geometry id");

            m_instance_data_map.at(geometry_id).append(color, world);
        }

        //-------------------------------------------------------------------------
        void instance_renderer::enable_solid_rendering(bool enable)
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
        void instance_renderer::enable_wireframe_rendering(bool enable)
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
        void instance_renderer::buffer_policy(render_buffer_policy render_buffer_policy)
        {
            m_buffer_policy = render_buffer_policy;
        }

        //-------------------------------------------------------------------------
        void instance_renderer::draw_policy(render_draw_policy render_draw_policy)
        {
            m_render_policy = render_draw_policy;
            m_render_fns.clear();

            switch (render_draw_policy)
            {
            case render_draw_policy::BUILD_IN:
                if (solid_rendering_supported())
                {
                    m_render_fns.push_back([&](instance_drawing_data& drawing_data) {
                        solid_render(drawing_data);
                    });
                }
                if (wireframe_rendering_supported())
                {
                    m_render_fns.push_back([&](instance_drawing_data& drawing_data) {
                        wireframe_render(drawing_data);
                    });
                }
                break;
            case render_draw_policy::CUSTOM:
                m_render_fns.push_back([&](instance_drawing_data& drawing_data) {
                    on_render(drawing_data);
                });
                break;
            }
        }

        //-------------------------------------------------------------------------
        void instance_renderer::user_shader_program(const std::string& tag)
        {
            if (shader_pool::has_shader(tag))
            {
                m_user_shader_tag = tag;
            }
        }

        //-------------------------------------------------------------------------
        void instance_renderer::reset_user_shader_program()
        {
            m_user_shader_tag = {};
        }

        //-------------------------------------------------------------------------
        bool instance_renderer::solid_rendering_supported() const
        {
            return m_rasterization_mode & internal::_solid;
        }

        //-------------------------------------------------------------------------
        bool instance_renderer::wireframe_rendering_supported() const
        {
            return m_rasterization_mode & internal::_wireframe;
        }

        //-------------------------------------------------------------------------
        bool instance_renderer::has_instance_data() const
        {
            return std::any_of(std::cbegin(m_instance_data_map), std::cend(m_instance_data_map),
                [](const auto& pair)
            {
                return pair.second.has_instance_data();
            });
        }

        //-------------------------------------------------------------------------
        bool instance_renderer::has_drawing_data() const
        {
            return std::any_of(std::cbegin(m_instance_data_map), std::cend(m_instance_data_map),
                [](const auto& pair)
            {
                return pair.second.has_drawing_data();
            });
        }

        //-------------------------------------------------------------------------
        u32 instance_renderer::shader_program() const
        {
            return shader_pool::get_shader_program(m_user_shader_tag.empty() ? m_shader_tag : m_user_shader_tag);
        }

        //-------------------------------------------------------------------------
        render_buffer_policy instance_renderer::buffer_policy() const
        {
            return m_buffer_policy;
        }

        //-------------------------------------------------------------------------
        render_draw_policy instance_renderer::draw_policy() const
        {
            return m_render_policy;
        }

        //-------------------------------------------------------------------------
        void instance_renderer::solid_render(instance_drawing_data& drawing_data)
        {
            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

            shaders::push_uniform(shader_program(), "u_wireframe", GL_FALSE);

            on_render(drawing_data);
        }

        //-------------------------------------------------------------------------
        void instance_renderer::wireframe_render(instance_drawing_data& drawing_data)
        {
            GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
            GL_CALL(glLineWidth(internal::_wireframe_linewidth));

            shaders::push_uniform(shader_program(), "u_wireframe", GL_TRUE);
            shaders::push_uniform(shader_program(), "u_wireframe_color", color::convert_color(internal::_wireframe_linecolor));

            on_render(drawing_data);
        }

        // Primitive Instance Renderer
        //-------------------------------------------------------------------------
        primitive_instance_renderer::primitive_instance_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag)
            :instance_renderer(layouts, layout_cout, shader_tag, false)
        {

        }

        //-------------------------------------------------------------------------
        primitive_instance_renderer::~primitive_instance_renderer() = default;

        //-------------------------------------------------------------------------
        void primitive_instance_renderer::on_render(instance_drawing_data& drawing_data)
        {
            auto inst = drawing_data.instance();
            if (inst != nullptr)
            {
                GLenum gl_topology = internal::topology(drawing_data.topology());

                GL_CALL(glBindVertexArray(drawing_data.vao()));

                #ifndef NDEBUG
                internal::check_drawing_type(inst, gl_topology);
                #endif

                if (inst->index_count() != 0)
                {
                    shaders::apply_uniforms(shader_program());
                    GL_CALL(glDrawElementsInstanced(gl_topology, inst->index_count(), internal::index_type(), nullptr, drawing_data.instance_count()));
                }
                else
                {
                    shaders::apply_uniforms(shader_program());
                    GL_CALL(glDrawArraysInstanced(gl_topology, 0, inst->vertex_count(), drawing_data.instance_count()));
                }

                GL_CALL(glBindVertexArray(0));
            }
        }

        //// Texture Instance Renderer
        ////-------------------------------------------------------------------------
        //texture_instance_renderer::texture_instance_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag)
        //    :instance_renderer(layouts, layout_cout, shader_tag, true)
        //{

        //}

        ////-------------------------------------------------------------------------
        //texture_instance_renderer::~texture_instance_renderer() = default;

        ////-------------------------------------------------------------------------
        //void texture_instance_renderer::on_render(instance_drawing_data& drawing_data)
        //{
        //    auto inst = drawing_data.instance();
        //    if (inst != nullptr)
        //    {
        //        GLenum gl_topology = internal::topology(drawing_data.topology());

        //        GL_CALL(glBindVertexArray(drawing_data.vao()));

        //        assert(inst->samplers());
        //        assert(inst->textures());

        //        shaders::push_uniform_array(shader_program(), "s_image", inst->sampler_count(), inst->samplers());

        //        #ifndef NDEBUG
        //        internal::check_drawing_type(inst, gl_topology);
        //        #endif

        //        s32 i = 0;
        //        s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
        //        for (int i = 0; i < inst->texture_count(); ++i)
        //        {
        //            GL_CALL(glActiveTexture(GL_TEXTURE0 + (offset * i)));
        //            GL_CALL(glBindTexture(GL_TEXTURE_2D, inst->textures()[i]));
        //        }

        //        if (inst->index_count() != 0)
        //        {
        //            shaders::apply_uniforms(shader_program());
        //            GL_CALL(glDrawElementsInstanced(gl_topology, inst->index_count(), internal::index_type(), nullptr, drawing_data.instance_count()));
        //        }
        //        else
        //        {
        //            shaders::apply_uniforms(shader_program());
        //            GL_CALL(glDrawArraysInstanced(gl_topology, 0, inst->vertex_count(), drawing_data.instance_count()));
        //        }

        //        GL_CALL(glBindVertexArray(0));
        //    }
        //}
    }
}