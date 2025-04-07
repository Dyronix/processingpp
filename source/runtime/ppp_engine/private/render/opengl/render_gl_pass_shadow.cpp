#include "render/render_pass_shadow.h"
#include "render/render_batch_renderer.h"
#include "render/render_batch_data_table.h"
#include "render/render_instance_renderer.h"
#include "render/render_instance_data_table.h"
#include "render/render_context.h"
#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/lights_pool.h"
#include "resources/shader_pool.h"
#include "resources/material.h"

#include "camera/camera_context.h"

#include "util/log.h"

#include <algorithm>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        static void push_all_shape_dependent_uniforms(resources::shader_program shader_program, const glm::mat4& vp)
        {
            shaders::push_uniform(shader_program->id(), string::store_sid("u_view_proj"), vp);
        }

        //-------------------------------------------------------------------------
        shadow_pass::shadow_pass(string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags, draw_mode draw_mode)
            :geometry_render_pass("shadow"_sid, shader_tag, framebuffer_tag, framebuffer_flags, draw_mode)
        {}
        //-------------------------------------------------------------------------
        shadow_pass::~shadow_pass() = default;

        //-------------------------------------------------------------------------
        void shadow_pass::begin_frame(const render_context& context)
        {
            assert(context && "Invalid render context");

            // When there are no lights we can early out
            if (lights_pool::directional_lights().empty())
            {
                return;
            }

            // Bind pass framebuffer
            framebuffer()->bind();

            // Configure OpenGL state.
            opengl::api::instance().disable(GL_BLEND);      

            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().cull_face(GL_BACK);

            opengl::api::instance().enable(GL_DEPTH_TEST);
            opengl::api::instance().depth_func(GL_LESS);

            opengl::api::instance().viewport(0, 0, framebuffer()->width(), framebuffer()->height());

            opengl::api::instance().clear_depth(1.0);
            opengl::api::instance().clear(GL_DEPTH_BUFFER_BIT);

            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_FILL);

            // Bind the pass shader
            opengl::api::instance().use_program(shader_program()->id());

            // Apply shape uniforms
            // For now we only support 1 light
            assert(lights_pool::directional_lights().size() == 1);

            auto& dir_light = lights_pool::directional_lights()[0];

            const glm::vec3 light_pos_active = -dir_light.direction * 500.0f;

            const f32 near_plane = 0.01f;
            const f32 far_plane = 1000.0f;

            const glm::mat4 light_active_p = glm::ortho(-framebuffer()->width() / 2.0f, framebuffer()->width() / 2.0f, -framebuffer()->height() / 2.0f, framebuffer()->height() / 2.0f, near_plane, far_plane);;
            const glm::mat4 light_active_v = glm::lookAt(light_pos_active, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            const glm::mat4 light_active_vp = light_active_p * light_active_v;

            push_all_shape_dependent_uniforms(shader_program(), light_active_vp);
        }

        //-------------------------------------------------------------------------
        void shadow_pass::render(const render_context& context)
        {
            // When there are no lights we can early out
            if (lights_pool::directional_lights().empty())
            {
                return;
            }

            bool batched_shading = batch_rendering_enabled();

            if (batched_shading)
            {
                shaders::apply_uniforms(shader_program()->id());

                for (auto& [key, batch] : *context.batch_data)
                {
                    if (key.cast_shadows)
                    {
                        batch_renderer::render(batch_render_strategy(), batch.get());
                    }
                }
            }
            else
            {
                shaders::apply_uniforms(shader_program()->id());

                for (auto& [key, instance] : *context.instance_data)
                {
                    if (key.cast_shadows)
                    {
                        instance_renderer::render(instance_render_strategy(), instance.get());
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        void shadow_pass::end_frame(const render_context& context)
        {
            // When there are no lights we can early out
            if (lights_pool::directional_lights().empty())
            {
                return;
            }

            // Unbind pass shader 
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            framebuffer()->unbind();
        }

        //-------------------------------------------------------------------------
        bool shadow_pass::should_render(const render_context& context) const
        {
            auto& dir_lights = lights_pool::directional_lights();
            auto num = std::count_if(std::cbegin(dir_lights), std::end(dir_lights),
                [](const lights_pool::directional_light& light)
            {
                return light.cast_shadows;
            });

            return num > 0;
        }
    }
}