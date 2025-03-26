#include "render/render_instance_renderer.h"
#include "render/render_item.h"
#include "render/render_shader_uniform_manager.h"
#include "render/render_features.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"

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

        // Instance Renderer
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
        instance_renderer::instance_renderer(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag)
            : base_renderer(shader_tag)
            , m_instance_data_map()
            , m_instance_layouts(instance_layouts)
            , m_instance_layout_count(instance_layout_count)
        {
            m_render_fns.clear();

            if (solid_rendering_supported())
            {
                m_render_fns.push_back([&](topology_type topology, instance_drawing_data& drawing_data) {
                    solid_render(topology, drawing_data);
                });
            }
            if (wireframe_rendering_supported())
            {
                m_render_fns.push_back([&](topology_type topology, instance_drawing_data& drawing_data) {
                    wireframe_render(topology, drawing_data);
                });
            }
        }

        //-------------------------------------------------------------------------
        instance_renderer::~instance_renderer() = default;

        //-------------------------------------------------------------------------
        void instance_renderer::begin()
        {
            for (auto& pair : m_instance_data_map)
            {
                pair.second.reset();
            }
        }

        //-------------------------------------------------------------------------
        void instance_renderer::render()
        {
            if (!has_drawing_data())
            {
                // No drawing data, early out
                return;
            }

            for (auto& pair : m_instance_data_map)
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
        void instance_renderer::append_drawing_data(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {
            if (m_instance_data_map.find(topology) == std::cend(m_instance_data_map))
            {
                m_instance_data_map.emplace(topology, instance_drawing_data(layouts(), layout_count(), m_instance_layouts, m_instance_layout_count));
            }

            m_instance_data_map.at(topology).append(item, color, world);
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
        void instance_renderer::solid_render(topology_type topology, instance_drawing_data& drawing_data)
        {
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_FILL);

            shaders::push_uniform(shader_program()->id(), string::store_sid("u_wireframe"), GL_FALSE);

            on_render(topology, drawing_data);
        }

        //-------------------------------------------------------------------------
        void instance_renderer::wireframe_render(topology_type topology, instance_drawing_data& drawing_data)
        {
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_LINE);
            opengl::api::instance().line_width(internal::_wireframe_linewidth);

            shaders::push_uniform(shader_program()->id(), string::store_sid("u_wireframe"), GL_TRUE);
            shaders::push_uniform(shader_program()->id(), string::store_sid("u_wireframe_color"), color::convert_color(internal::_wireframe_linecolor));

            on_render(topology, drawing_data);
        }

        // Primitive Instance Renderer
        //-------------------------------------------------------------------------
        primitive_instance_renderer::primitive_instance_renderer(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag)
            :instance_renderer(instance_layouts, instance_layout_count, shader_tag)
        {

        }

        //-------------------------------------------------------------------------
        primitive_instance_renderer::~primitive_instance_renderer() = default;

        //-------------------------------------------------------------------------
        void primitive_instance_renderer::on_render(topology_type topology, instance_drawing_data& drawing_data)
        {
            auto inst = drawing_data.first_instance();
            if (inst != nullptr)
            {
                while (inst != nullptr)
                {
                    inst->bind();
                    inst->submit();
                    inst->draw(topology);
                    inst->unbind();

                    inst = drawing_data.next_instance();
                }
            }
        }

        // Texture Instance Renderer
        //-------------------------------------------------------------------------
        texture_instance_renderer::texture_instance_renderer(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag)
            :instance_renderer(instance_layouts, instance_layout_count, shader_tag)
        {

        }

        //-------------------------------------------------------------------------
        texture_instance_renderer::~texture_instance_renderer() = default;

        //-------------------------------------------------------------------------
        void texture_instance_renderer::on_render(topology_type topology, instance_drawing_data& drawing_data)
        {
            auto inst = drawing_data.first_instance();
            if (inst != nullptr)
            {
                while (inst != nullptr)
                {
                    inst->bind();
                    inst->submit();
                    inst->draw(topology);
                    inst->unbind();

                    inst = drawing_data.next_instance();
                }
            }
        }
    }
}