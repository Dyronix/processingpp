#include "render/render_forward_shading_pass.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"
#include "render/render_context.h"
#include "render/render_scissor.h"
#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/lights_pool.h"
#include "resources/shader_pool.h"
#include "resources/material.h"

#include "camera/camera_context.h"

#include "string/string_conversions.h"

#include "util/log.h"
#include "util/brush.h"

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
        static void push_all_light_dependent_uniforms(resources::shader_program shader_program, const glm::vec3& camera_position)
        {
            if (lights_pool::point_lights().empty() == false || lights_pool::directional_lights().empty() == false)
            {
                shaders::push_uniform(shader_program->id(), string::store_sid("u_view_position"), camera_position);
            }

            if (lights_pool::point_lights().empty() == false)
            {
                constexpr u64 max_nr_point_lights = 8;

                u64 nr_active_point_lights = std::min(lights_pool::point_lights().size(), max_nr_point_lights);

                shaders::push_uniform(shader_program->id(), string::store_sid("u_num_point_lights"), static_cast<s32>(nr_active_point_lights));

                for (s32 i = 0; i < nr_active_point_lights; ++i)
                {
                    auto& point_light = lights_pool::point_lights()[i];

                    std::string base_name = std::string("u_point_lights[") + string::to_string<std::string>(i) + std::string("]");

                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".position"), point_light.position);
                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".ambient"), point_light.ambient);
                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".diffuse"), point_light.diffuse);
                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".specular"), point_light.specular);
                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".specular_enabled"), point_light.specular_enabled);

                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".constant"), point_light.constant);
                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".linear"), point_light.linear);
                    shaders::push_uniform(shader_program->id(), string::store_sid(base_name + ".quadratic"), point_light.quadratic);
                }
            }

            if (lights_pool::directional_lights().empty() == false)
            {
                for (auto& dir_light : lights_pool::directional_lights())
                {
                    shaders::push_uniform(shader_program->id(), string::store_sid("u_directional_light.direction"), dir_light.direction);
                    shaders::push_uniform(shader_program->id(), string::store_sid("u_directional_light.ambient"), dir_light.ambient);
                    shaders::push_uniform(shader_program->id(), string::store_sid("u_directional_light.diffuse"), dir_light.diffuse);
                    shaders::push_uniform(shader_program->id(), string::store_sid("u_directional_light.specular"), dir_light.specular);
                    shaders::push_uniform(shader_program->id(), string::store_sid("u_directional_light.specular_enabled"), dir_light.specular_enabled);
                }
            }
        }
        //-------------------------------------------------------------------------
        static void push_all_shadow_dependent_uniforms(resources::shader_program shader_program, const glm::mat4& lightvp)
        {
            if (lights_pool::directional_lights().empty() == false)
            {
                if (lights_pool::has_directional_lights_with_shadow())
                {
                    shaders::push_uniform(shader_program->id(), string::store_sid("u_light_vp"), lightvp);
                }
            }
        }

        //-------------------------------------------------------------------------
        forward_shading_pass::forward_shading_pass(const string::string_id shader_tag)
            :render_pass(shader_tag)
        {}
        //-------------------------------------------------------------------------
        forward_shading_pass::~forward_shading_pass() = default;

        //-------------------------------------------------------------------------
        void forward_shading_pass::begin_frame(const render_context& context)
        {
            assert(context && "Invalid render context");

            // Bind pass framebuffer
            auto forward_framebuffer = framebuffer_pool::get(framebuffer_pool::tags::forward_shading(), framebuffer_flags::COLOR | framebuffer_flags::DEPTH);

            forward_framebuffer->bind();

            // Configure OpenGL state.
            opengl::api::instance().disable(GL_BLEND);

            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().cull_face(GL_BACK);

            opengl::api::instance().enable(GL_DEPTH_TEST);
            opengl::api::instance().depth_func(GL_LEQUAL); // Optional: Use GL_LEQUAL for matching precision
            opengl::api::instance().depth_mask(GL_FALSE); // Disable depth writes

            opengl::api::instance().viewport(0, 0, forward_framebuffer->width(), forward_framebuffer->height());

            opengl::api::instance().clear_color(0.0f, 0.0f, 0.0f, 1.0f);
            opengl::api::instance().clear_depth(1.0);
            opengl::api::instance().clear(GL_COLOR_BUFFER_BIT);

            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_FILL);

            // Set the background clear color from the brush.
            glm::vec4 bg_color = brush::background();

            opengl::api::instance().clear_color(bg_color.r, bg_color.g, bg_color.b, bg_color.a);

            // Configure scissor test if enabled.
            if (context.scissor->enable)
            {
                opengl::api::instance().enable(GL_SCISSOR_TEST);
                opengl::api::instance().scissor(
                    std::clamp(context.scissor->x, 0, forward_framebuffer->width()),
                    std::clamp(context.scissor->y, 0, forward_framebuffer->height()),
                    std::clamp(context.scissor->width, framebuffer::min_framebuffer_width(), forward_framebuffer->width()),
                    std::clamp(context.scissor->height, framebuffer::min_framebuffer_height(), forward_framebuffer->height())
                );
            }
            else
            {
                opengl::api::instance().disable(GL_SCISSOR_TEST);
            }

            // Clear buffers after scissor state is set.
            opengl::api::instance().clear(GL_COLOR_BUFFER_BIT);

            // Bind the pass shader
            opengl::api::instance().use_program(shader_program()->id());

            // Apply shape uniforms
            // For now we only support 1 light
            assert(lights_pool::directional_lights().size() == 1);

            auto& dir_light = lights_pool::directional_lights()[0];

            const glm::vec3 light_pos_active = -dir_light.direction * 500.0f;
            const glm::vec3 light_tar_active = glm::vec3(0.0f);

            const auto shadow_framebuffer = framebuffer_pool::get(framebuffer_pool::tags::shadow_map(), framebuffer_flags::SAMPLED_DEPTH);

            const f32 near_plane = 0.01f;
            const f32 far_plane = 1000.0f;

            const glm::mat4 light_active_p = glm::ortho(-shadow_framebuffer->width() / 2.0f, shadow_framebuffer->width() / 2.0f, -shadow_framebuffer->height() / 2.0f, shadow_framebuffer->height() / 2.0f, near_plane, far_plane);;
            const glm::mat4 light_active_v = glm::lookAt(light_pos_active, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            const glm::mat4 light_active_vp = light_active_p * light_active_v;

            const glm::vec3& cam_pos_active = context.camera_context->camera_position_active;
            const glm::vec3& cam_tar_active = context.camera_context->camera_lookat_active;

            const glm::mat4& cam_active_p = context.camera_context->mat_proj_active;
            const glm::mat4& cam_active_v = context.camera_context->mat_view_active;

            const glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            push_all_shape_dependent_uniforms(shader_program(), cam_active_vp);
            push_all_light_dependent_uniforms(shader_program(), cam_pos_active);
            push_all_shadow_dependent_uniforms(shader_program(), light_active_vp);
        }

        //-------------------------------------------------------------------------
        void forward_shading_pass::render(const render_context& context)
        {
            shaders::apply_uniforms(shader_program()->id());

            const auto& samplers = material()->samplers();
            const auto& textures = material()->textures();

            u64 offset = GL_TEXTURE1 - GL_TEXTURE0;

            for (auto& pair : *context.batch_renderers)
            {
                bool has_texture_support = pair.second->has_texture_support();

                u64 sampler_size = has_texture_support ? samplers.size() : 0;
                u64 texture_size = has_texture_support ? textures.size() : 0;

                if (has_texture_support)
                {
                    shaders::push_uniform_array(shader_program()->id(), string::store_sid("u_image_samplers"), sampler_size, samplers.data());

                    for (s32 i = 0; i < texture_size; ++i)
                    {
                        opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * i));
                        opengl::api::instance().bind_texture(GL_TEXTURE_2D, textures[i]);
                    }
                }

                if (lights_pool::directional_lights().empty() == false)
                {
                    if (lights_pool::has_directional_lights_with_shadow())
                    {
                        const framebuffer* shadow_framebuffer = static_cast<const framebuffer*>(framebuffer_pool::get(framebuffer_pool::tags::shadow_map(), framebuffer_flags::SAMPLED_DEPTH));

                        shaders::push_uniform(shader_program()->id(), string::store_sid("u_shadows_enabled"), 1);
                        shaders::push_uniform(shader_program()->id(), string::store_sid("u_shadow_map"), (s32)sampler_size);

                        opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * texture_size));
                        opengl::api::instance().bind_texture(GL_TEXTURE_2D, shadow_framebuffer->depth_attachment());
                    }
                    else
                    {
                        shaders::push_uniform(shader_program()->id(), string::store_sid("u_shadows_enabled"), 0);
                    }
                }
                else
                {
                    shaders::push_uniform(shader_program()->id(), string::store_sid("u_shadows_enabled"), 0);
                }

                pair.second->render();
            }

            for (auto& pair : *context.instance_renderers)
            {
                bool has_texture_support = pair.second->has_texture_support();

                u64 sampler_size = has_texture_support ? samplers.size() : 0;
                u64 texture_size = has_texture_support ? textures.size() : 0;

                if (has_texture_support)
                {
                    shaders::push_uniform_array(shader_program()->id(), string::store_sid("u_image_samplers"), sampler_size, samplers.data());

                    for (s32 i = 0; i < texture_size; ++i)
                    {
                        opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * i));
                        opengl::api::instance().bind_texture(GL_TEXTURE_2D, textures[i]);
                    }
                }

                pair.second->render();
            }
        }

        //-------------------------------------------------------------------------
        void forward_shading_pass::end_frame(const render_context& context)
        {
            // Reset state
            opengl::api::instance().depth_mask(GL_TRUE);
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            auto framebuffer = framebuffer_pool::get(framebuffer_pool::tags::forward_shading(), framebuffer_flags::COLOR | framebuffer_flags::DEPTH);

            framebuffer->unbind();
        }
    }
}