#include "render/render_batch_renderer.h"
#include "render/render_item.h"
#include "render/render_shader_uniform_manager.h"
#include "render/render_features.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "resources/lights_pool.h"
#include "resources/framebuffer_pool.h"

#include "util/log.h"
#include "util/color_ops.h"

#include "string/string_conversions.h"

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
            static void push_all_light_dependent_uniforms(u32 shader_program, const glm::vec3& camera_position, const glm::vec3& camera_target)
            {
                if (lights_pool::point_lights().empty() == false
                 || lights_pool::directional_lights().empty() == false)
                {
                    shaders::push_uniform(shader_program, string::store_sid("u_view_position"), camera_position);
                    //shaders::push_uniform(shader_program, string::store_sid("u_view_target"), camera_target);
                }
            }

            //-------------------------------------------------------------------------
            static void push_all_shadow_dependent_uniforms(u32 shader_program, const glm::mat4& lightvp)
            {
                if (lights_pool::directional_lights().empty() == false)
                {
                    if (lights_pool::has_directional_lights_with_shadow())
                    {
                        shaders::push_uniform(shader_program, string::store_sid("u_light_vp"), lightvp);
                    }
                }
            }

            //-------------------------------------------------------------------------
            static void push_all_light_uniforms(u32 shader_program)
            {
                if (lights_pool::point_lights().empty() == false)
                {
                    constexpr u64 max_nr_point_lights = 8;

                    u64 nr_active_point_lights = std::min(lights_pool::point_lights().size(), max_nr_point_lights);

                    shaders::push_uniform(shader_program, string::store_sid("u_num_point_lights"), static_cast<s32>(nr_active_point_lights));

                    for (s32 i = 0; i < nr_active_point_lights; ++i)
                    {
                        auto& point_light = lights_pool::point_lights()[i];

                        std::string base_name = std::string("u_point_lights[") + string::to_string<std::string>(i) + std::string("]");

                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".position"), point_light.position);
                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".ambient"), point_light.ambient);
                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".diffuse"), point_light.diffuse);
                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".specular"), point_light.specular);
                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".specular_enabled"), point_light.specular_enabled);

                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".constant"), point_light.constant);
                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".linear"), point_light.linear);
                        shaders::push_uniform(shader_program, string::store_sid(base_name + ".quadratic"), point_light.quadratic);
                    }
                }

                if (lights_pool::directional_lights().empty() == false)
                {
                    for (auto& dir_light : lights_pool::directional_lights())
                    {
                        shaders::push_uniform(shader_program, string::store_sid("u_directional_light.direction"), dir_light.direction);
                        shaders::push_uniform(shader_program, string::store_sid("u_directional_light.ambient"), dir_light.ambient);
                        shaders::push_uniform(shader_program, string::store_sid("u_directional_light.diffuse"), dir_light.diffuse);
                        shaders::push_uniform(shader_program, string::store_sid("u_directional_light.specular"), dir_light.specular);
                        shaders::push_uniform(shader_program, string::store_sid("u_directional_light.specular_enabled"), dir_light.specular_enabled);
                    }
                }
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
        batch_renderer::batch_renderer(string::string_id shader_tag)
            : base_renderer(shader_tag)
            , m_drawing_data_map()
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
        void batch_renderer::render(const glm::vec3& camera_position, const glm::vec3& camera_target, const glm::mat4& lightvp, const glm::mat4& vp)
        {
            if (!has_drawing_data())
            {
                // No drawing data, early out
                return;
            }

            const auto& program = shader_program();

            opengl::api::instance().use_program(program->id());

            if (program->shading_model() == shading_model_type::LIT)
            {
                internal::push_all_light_dependent_uniforms(program->id(), camera_position, camera_target);
                internal::push_all_shadow_dependent_uniforms(program->id(), lightvp);
            }

            shaders::push_uniform(program->id(), string::store_sid("u_view_proj"), vp);

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

            opengl::api::instance().use_program(0);
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
                s32 max_ver = max_vertices(topology);
                s32 max_idx = max_indices(topology);

                m_drawing_data_map.emplace(topology, batch_drawing_data(max_ver, max_idx, layouts(), layout_count(), m_buffer_policy));
            }

            m_drawing_data_map.at(topology).append(item, color, world);
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
        bool batch_renderer::has_drawing_data() const
        {
            return std::any_of(std::cbegin(m_drawing_data_map), std::cend(m_drawing_data_map),
                [](const auto& pair)
            {
                return pair.second.has_drawing_data();
            });
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
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_FILL);

            shaders::push_uniform(shader_program()->id(), string::store_sid("u_wireframe"), GL_FALSE);

            on_render(topology, drawing_data);
        }

        //-------------------------------------------------------------------------
        void batch_renderer::wireframe_render(topology_type topology, batch_drawing_data& drawing_data)
        {
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_LINE);
            opengl::api::instance().line_width(internal::_wireframe_linewidth);

            shaders::push_uniform(shader_program()->id(), string::store_sid("u_wireframe"), GL_TRUE);
            shaders::push_uniform(shader_program()->id(), string::store_sid("u_wireframe_color"), color::convert_color(internal::_wireframe_linecolor));

            on_render(topology, drawing_data);
        }

        // Primitive Batch Renderer
        //-------------------------------------------------------------------------
        primitive_batch_renderer::primitive_batch_renderer(string::string_id shader_tag)
            :batch_renderer(shader_tag)
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
                const auto& program = shader_program();

                while (batch != nullptr)
                {
                    batch->bind();

                    if (program->shading_model() == shading_model_type::LIT)
                    {
                        internal::push_all_light_uniforms(program->id());

                        if (lights_pool::directional_lights().empty() == false)
                        {
                            if (lights_pool::has_directional_lights_with_shadow())
                            {
                                const framebuffer* shadow_framebuffer = static_cast<const framebuffer*>(framebuffer_pool::get(framebuffer_pool::tags::shadow_map(), framebuffer_flags::SAMPLED_DEPTH));

                                shaders::push_uniform(program->id(), string::store_sid("u_shadows_enabled"), 1);
                                shaders::push_uniform(program->id(), string::store_sid("u_shadow_map"), 0);

                                opengl::api::instance().activate_texture(GL_TEXTURE0);
                                opengl::api::instance().bind_texture(GL_TEXTURE_2D, shadow_framebuffer->depth_attachment());
                            }
                        }
                    }

                    batch->submit();
                    batch->draw(topology, program->id());
                    batch->unbind();

                    batch = drawing_data.next_batch();
                }
            }
        }

        // Texture Batch Renderer
        //-------------------------------------------------------------------------
        texture_batch_renderer::texture_batch_renderer(string::string_id shader_tag)
            :batch_renderer(shader_tag)
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
                const auto& samplers = material()->samplers();
                const auto& textures = material()->textures();

                const auto& program = shader_program();

                while (batch != nullptr)
                {
                    batch->bind();
                 
                    internal::push_all_light_uniforms(program->id());

                    if (program->shading_model() == shading_model_type::LIT)
                    {
                        shaders::push_uniform_array(program->id(), string::store_sid("s_images"), samplers.size(), samplers.data());
                    }

                    s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                    for (s32 i = 0; i < textures.size(); ++i)
                    {
                        opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * i));
                        opengl::api::instance().bind_texture(GL_TEXTURE_2D, textures[i]);
                    }

                    if (lights_pool::directional_lights().empty() == false)
                    {
                        if (lights_pool::has_directional_lights_with_shadow())
                        {
                            const framebuffer* shadow_framebuffer = static_cast<const framebuffer*>(framebuffer_pool::get(framebuffer_pool::tags::shadow_map(), framebuffer_flags::SAMPLED_DEPTH));

                            shaders::push_uniform(program->id(), string::store_sid("u_shadow_map"), (s32)samplers.size());

                            opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * textures.size()));
                            opengl::api::instance().bind_texture(GL_TEXTURE_2D, shadow_framebuffer->depth_attachment());
                        }
                    }

                    batch->submit();
                    batch->draw(topology, program->id());
                    batch->unbind();

                    batch = drawing_data.next_batch();
                }
            }
        }
    }
}