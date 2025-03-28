#include "render/render_unlit_pass.h"
#include "render/render_batch_renderer.h"
#include "render/render_batch_data_table.h"
#include "render/render_instance_renderer.h"
#include "render/render_instance_data_table.h"
#include "render/render_context.h"
#include "render/render_scissor.h"
#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/shader_pool.h"

#include "camera/camera_context.h"

#include "util/log.h"
#include "util/brush.h"

#include <algorithm>
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
        unlit_pass::unlit_pass(const string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags)
            :render_pass(shader_tag, framebuffer_tag, framebuffer_flags)
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
                    std::clamp(context.scissor->x, 0, framebuffer()->width()),
                    std::clamp(context.scissor->y, 0, framebuffer()->height()),
                    std::clamp(context.scissor->width, framebuffer::min_framebuffer_width(), framebuffer()->width()),
                    std::clamp(context.scissor->height, framebuffer::min_framebuffer_height(), framebuffer()->height())
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
            const glm::vec3& cam_pos_active = context.camera_context->camera_position_active;
            const glm::vec3& cam_tar_active = context.camera_context->camera_lookat_active;

            const glm::mat4& cam_active_p = context.camera_context->mat_proj_active;
            const glm::mat4& cam_active_v = context.camera_context->mat_view_active;

            const glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            push_all_shape_dependent_uniforms(shader_program(), cam_active_vp);
        }

        //-------------------------------------------------------------------------
        void unlit_pass::render(const render_context& context)
        {
            shaders::apply_uniforms(shader_program()->id());

            for (auto& pair : *context.batch_data)
            {
                if (shader_pool::shading_model_for_shader(shader_tag()) != shader_pool::shading_model_for_shader(pair.first))
                {
                    continue;
                }

                batch_renderer::render(batch_render_strategy(), pair.second.get());
            }
            for (auto& pair : *context.instance_data)
            {
                if (shader_pool::shading_model_for_shader(shader_tag()) != shader_pool::shading_model_for_shader(pair.first))
                {
                    continue;
                }

                instance_renderer::render(instance_render_strategy(), pair.second.get());
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