#include "render/render_ui_pass.h"
#include "render/render_batch_renderer.h"
#include "render/render_context.h"
#include "render/render_scissor.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"

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
        ui_pass::ui_pass(const string::string_id shader_tag)
            :render_pass(shader_tag)
        {}
        //-------------------------------------------------------------------------
        ui_pass::~ui_pass() = default;

        //-------------------------------------------------------------------------
        void ui_pass::begin_frame(const render_context& context)
        {
            context.font_renderer->begin();

            auto framebuffer = framebuffer_pool::get(framebuffer_pool::tags::ui(), framebuffer_flags::COLOR);

            framebuffer->bind();

            // Set viewport and clear the framebuffer.
            opengl::api::instance().viewport(0, 0, framebuffer->width(), framebuffer->height());
            opengl::api::instance().clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            opengl::api::instance().clear(GL_COLOR_BUFFER_BIT);

            // Set the background clear color from the brush.
            glm::vec4 bg_color = brush::background();
            opengl::api::instance().clear_color(bg_color.r, bg_color.g, bg_color.b, bg_color.a);

            // Configure scissor test if enabled.
            if (context.scissor->enable)
            {
                opengl::api::instance().enable(GL_SCISSOR_TEST);
                opengl::api::instance().scissor(
                    std::clamp(context.scissor->x, 0, framebuffer->width()),
                    std::clamp(context.scissor->y, 0, framebuffer->height()),
                    std::clamp(context.scissor->width, framebuffer::min_framebuffer_width(), framebuffer->width()),
                    std::clamp(context.scissor->height, framebuffer::min_framebuffer_height(), framebuffer->height())
                );
            }
            else
            {
                opengl::api::instance().disable(GL_SCISSOR_TEST);
            }

            // Clear buffers after scissor state is set.
            opengl::api::instance().clear(GL_COLOR_BUFFER_BIT);
        }

        //-------------------------------------------------------------------------
        void ui_pass::render(const render_context& context)
        {
            //auto& cam_pos_font = context.camera_context->camera_position_font;
            //auto& cam_tar_font = context.camera_context->camera_lookat_font;

            //opengl::api::instance().disable(GL_BLEND);
            //opengl::api::instance().enable(GL_CULL_FACE);
            //opengl::api::instance().enable(GL_DEPTH_TEST);

            //opengl::api::instance().blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //opengl::api::instance().cull_face(GL_BACK);
            //opengl::api::instance().depth_func(GL_LESS);

            //const glm::mat4& cam_font_p = context.camera_context->mat_proj_font;
            //const glm::mat4& cam_font_v = context.camera_context->mat_view_font;
            //glm::mat4 cam_font_vp = cam_font_p * cam_font_v;

            //context.font_renderer->render(cam_pos_font, cam_tar_font, glm::mat4(1.0f), cam_font_vp);
        }

        //-------------------------------------------------------------------------
        void ui_pass::end_frame(const render_context& context)
        {
            context.font_renderer->end();
        }
    }
}