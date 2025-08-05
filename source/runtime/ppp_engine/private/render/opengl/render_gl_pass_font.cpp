#include "render/render_pass_font.h"
#include "render/render_batch_renderer.h"
#include "render/render_batch_data_table.h"
#include "render/render_context.h"
#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/shader_pool.h"
#include "resources/material.h"

#include "camera/camera_context.h"
#include "camera/camera_manager.h"

#include <glad/glad.h>

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
        static void push_batch_uniforms(const batch_data_table* data_table, const resources::shader_program& shader_program, const resources::sampler_ids& samplers, const resources::texture_ids& textures)
        {
            const u64 offset = GL_TEXTURE1 - GL_TEXTURE0;

            const u64 sampler_size = samplers.size();
            const u64 texture_size = textures.size();

            if (!textures.empty())
            {
                shaders::push_uniform_array(shader_program->id(), string::store_sid("u_image_samplers"), sampler_size, samplers.data());

                for (u64 i = 0; i < texture_size; ++i)
                {
                    opengl::api::instance().activate_texture(GL_TEXTURE0 + (offset * i));
                    opengl::api::instance().bind_texture(GL_TEXTURE_2D, textures[i]);
                }
            }
        }

        //-------------------------------------------------------------------------
        font_pass::font_pass(const tag<unlit::font> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags)
            :geometry_render_pass("font"_sid, decltype(shader_tag)::batched(), framebuffer_tag, framebuffer_flags, draw_mode::BATCHED)
        {
        }
        //-------------------------------------------------------------------------
        font_pass::~font_pass() = default;

        //-------------------------------------------------------------------------
        void font_pass::begin_frame(const render_context& context)
        {
            framebuffer()->bind();

            // Configure OpenGL state.
            opengl::api::instance().enable(GL_BLEND);
            opengl::api::instance().blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().cull_face(GL_BACK);

            opengl::api::instance().disable(GL_DEPTH_TEST);
            opengl::api::instance().depth_mask(GL_FALSE); // Disable depth writes

            opengl::api::instance().viewport(0, 0, framebuffer()->width(), framebuffer()->height());
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_FILL);

            // Bind the pass shader
            opengl::api::instance().use_program(shader_program()->id());

            // Apply shape uniforms
            const glm::mat4& cam_active_p = camera_manager::get_proj(camera_manager::tags::font());
            const glm::mat4& cam_active_v = camera_manager::get_view(camera_manager::tags::font());

            const glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            push_all_shape_dependent_uniforms(shader_program(), cam_active_vp);

            const auto& samplers = material()->samplers();
            const auto& textures = material()->textures();

            push_batch_uniforms(context.font_batch_data, shader_program(), samplers, textures);
        }

        //-------------------------------------------------------------------------
        void font_pass::render(const render_context& context)
        {
            shaders::apply_uniforms(shader_program()->id());

            batch_renderer::render(batch_render_strategy(), context.font_batch_data);
        }

        //-------------------------------------------------------------------------
        void font_pass::end_frame(const render_context& context)
        {
            // Reset state
            opengl::api::instance().disable(GL_BLEND);
            opengl::api::instance().depth_mask(GL_TRUE);
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            framebuffer()->unbind();
        }

        //-------------------------------------------------------------------------
        bool font_pass::should_render(const render_context& context) const
        {
            bool can_draw = false;

            if (drawing_mode() == draw_mode::BATCHED || drawing_mode() == draw_mode::AUTO)
            {
                can_draw |= context.font_batch_data != nullptr && context.font_batch_data->empty() == false;
            }

            return can_draw;
        }
    }
}
