#include "render/render_pass_unlit.h"
#include "render/render_batch_renderer.h"
#include "render/render_batch_data_table.h"
#include "render/render_instance_data_table.h"
#include "render/render_instance_renderer.h"
#include "render/render_context.h"
#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/shader_pool.h"
#include "resources/material.h"
#include "resources/texture_pool.h"

#include "camera/camera_context.h"

#include "util/log.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        static void push_batch_uniforms(const batch_data_table* data_table, const resources::shader_program& shader_program, const resources::sampler_ids& samplers, const resources::texture_ids& textures)
        {
            const bool has_texture_support = data_table->has_texture_support();

            if (has_texture_support)
            {
                const u64 offset = GL_TEXTURE1 - GL_TEXTURE0;
                const u64 sampler_size = samplers.size();
                const u64 texture_size = textures.size();
                const u64 max_tex = static_cast<u64>(max_textures());

                // build a full length sampler array, padding with the white slot
                std::vector<s32> padded_samplers(max_tex);
                for (u64 i = 0; i < max_tex; ++i)
                {
                    padded_samplers[i] = i < sampler_size ? samplers[i] : texture_pool::reserved_white_slot();
                }

                shaders::push_uniform_array(shader_program->id(), string::store_sid("u_image_samplers"), padded_samplers.size(), padded_samplers.data());

                const u64 reserved_slot = texture_pool::reserved_white_slot();
                opengl::api::instance().activate_texture(GL_TEXTURE0 + offset * reserved_slot);
                opengl::api::instance().bind_texture(GL_TEXTURE_2D, texture_pool::image_solid_white()->image_id);

                for (u64 i = 0; i < texture_size; ++i)
                {
                    opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * i));
                    opengl::api::instance().bind_texture(GL_TEXTURE_2D, textures[i]);
                }
            }
        }
        //-------------------------------------------------------------------------
        static void push_instance_uniforms(const instance_data_table* data_table, const resources::shader_program& shader_program, const resources::sampler_ids& samplers, const resources::texture_ids& textures)
        {
            const bool has_texture_support = data_table->has_texture_support();

            if (has_texture_support)
            {
                constexpr u64 offset = GL_TEXTURE1 - GL_TEXTURE0;
                const u64 sampler_size = samplers.size();
                const u64 texture_size = textures.size();
                const u64 max_tex = static_cast<u64>(max_textures());

                // build a full length sampler array, padding with the white slot
                std::vector<s32> padded_samplers(max_tex);
                for (u64 i = 0; i < max_tex; ++i)
                {
                    padded_samplers[i] = i < sampler_size ? samplers[i] : texture_pool::reserved_white_slot();
                }

                shaders::push_uniform_array(shader_program->id(), string::store_sid("u_image_samplers"), padded_samplers.size(), padded_samplers.data());

                const u64 reserved_slot = texture_pool::reserved_white_slot();
                opengl::api::instance().activate_texture(GL_TEXTURE0 + offset * reserved_slot);
                opengl::api::instance().bind_texture(GL_TEXTURE_2D, texture_pool::image_solid_white()->image_id);

                for (u64 i = 0; i < texture_size; ++i)
                {
                    opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * i));
                    opengl::api::instance().bind_texture(GL_TEXTURE_2D, textures[i]);
                }
            }
        }

        //-------------------------------------------------------------------------
        static void push_all_shape_dependent_uniforms(resources::shader_program shader_program, const glm::mat4& vp)
        {
            shaders::push_uniform(shader_program->id(), string::store_sid("u_wireframe"), 0);
            shaders::push_uniform(shader_program->id(), string::store_sid("u_view_proj"), vp);
        }

        //-------------------------------------------------------------------------
        unlit_pass::unlit_pass(string::string_id shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags, draw_mode draw_mode)
            :geometry_render_pass("unlit"_sid, shader_tag, framebuffer_tag, framebuffer_flags, draw_mode)
        {}
        //-------------------------------------------------------------------------
        unlit_pass::~unlit_pass() = default;

        //-------------------------------------------------------------------------
        void unlit_pass::begin_frame(const render_context& context)
        {
            framebuffer()->bind();

            // Configure OpenGL state.
            opengl::api::instance().disable(GL_BLEND);

            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().cull_face(GL_BACK);

            opengl::api::instance().enable(GL_DEPTH_TEST);
            opengl::api::instance().depth_func(GL_LEQUAL); // Optional: Use GL_LEQUAL for matching precision
            opengl::api::instance().depth_mask(GL_FALSE); // Disable depth writes

            opengl::api::instance().viewport(0, 0, framebuffer()->width(), framebuffer()->height());
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_FILL);

            // Bind the pass shader
            opengl::api::instance().use_program(shader_program()->id());

            // Apply shape uniforms
            const glm::mat4& cam_active_p = context.camera_context->mat_proj_active;
            const glm::mat4& cam_active_v = context.camera_context->mat_view_active;

            const glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            push_all_shape_dependent_uniforms(shader_program(), cam_active_vp);

            bool batched_shading = batch_rendering_enabled();

            const auto& samplers = material()->samplers();
            const auto& textures = material()->textures();

            if (batched_shading)
            {
                for (auto& [key, batch] : *context.batch_data)
                {
                    if (key.shader_tag == shader_tag())
                    {
                        push_batch_uniforms(batch.get(), shader_program(), samplers, textures);
                    }
                }
            }
            else
            {
                for (auto& [key, instance] : *context.instance_data)
                {
                    if (key.shader_tag == shader_tag())
                    {
                        push_instance_uniforms(instance.get(), shader_program(), samplers, textures);
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        void unlit_pass::render(const render_context& context)
        {
            bool batched_shading = batch_rendering_enabled();

            if (batched_shading)
            {
                shaders::apply_uniforms(shader_program()->id());

                for (auto& [key, batch] : *context.batch_data)
                {
                    if (key.shader_tag == shader_tag())
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
                    if (key.shader_tag == shader_tag())
                    {
                        instance_renderer::render(instance_render_strategy(), instance.get());
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        void unlit_pass::end_frame(const render_context& context)
        {
            // Reset state
            opengl::api::instance().depth_mask(GL_TRUE);
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            framebuffer()->unbind();
        }
    }
}