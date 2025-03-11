#include "render/render_forward_shading_pass.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"
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
        forward_shading_pass::~forward_shading_pass() = default;

        //-------------------------------------------------------------------------
        void forward_shading_pass::begin_frame(const render_context& context)
        {
            assert(context && "Invalid render context");

            for (auto& pair : *context.batch_renderers)
            {
                pair.second->begin();
            }
            for (auto& pair : *context.instance_renderers)
            {
                pair.second->begin();
            }

            // Bind the main framebuffer.
            auto framebuffer = framebuffer_pool::bind({ framebuffer_pool::tags::forward_shading(), true });

            // Set viewport and clear the framebuffer.
            opengl::api::instance().viewport(0, 0, framebuffer->width(), framebuffer->height());
            opengl::api::instance().clear_color(0.0f, 0.0f, 0.0f, 1.0f);
            opengl::api::instance().clear_depth(1.0);
            opengl::api::instance().clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
            opengl::api::instance().clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        //-------------------------------------------------------------------------
        void forward_shading_pass::render(const render_context& context)
        {
            // Retrieve camera parameters.
            auto& cam_pos_active = context.camera_context->camera_position_active;
            auto& cam_tar_active = context.camera_context->camera_lookat_active;

            // Configure OpenGL state.
            opengl::api::instance().disable(GL_BLEND);
            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().enable(GL_DEPTH_TEST);

            opengl::api::instance().blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            opengl::api::instance().cull_face(GL_BACK);
            opengl::api::instance().depth_func(GL_LESS);

            // Render batched and instanced geometry.
            const glm::mat4& cam_active_p = context.camera_context->mat_proj_active;
            const glm::mat4& cam_active_v = context.camera_context->mat_view_active;

            glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            for (auto& pair : *context.batch_renderers)
            {
                pair.second->render(cam_pos_active, cam_tar_active, cam_active_vp);
            }

            for (auto& pair : *context.instance_renderers)
            {
                pair.second->render(cam_pos_active, cam_tar_active, cam_active_vp);
            }

            // Blit the rendered image from the forward shading framebuffer to the system framebuffer.
            auto target_framebuffer = framebuffer_pool::get({ framebuffer_pool::tags::forward_shading(), true, false});
            auto system_framebuffer = framebuffer_pool::get_system();

            target_framebuffer->bind(framebuffer_bound_target::READ);
            system_framebuffer->bind(framebuffer_bound_target::WRITE);

            opengl::api::instance().blit_framebuffer(
                0,
                0,
                target_framebuffer->width(),
                target_framebuffer->height(),
                0,
                0,
                system_framebuffer->width(),
                system_framebuffer->height(),
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST
            );

            system_framebuffer->bind();
        }

        //-------------------------------------------------------------------------
        void forward_shading_pass::end_frame(const render_context& context)
        {
            framebuffer_pool::unbind({ framebuffer_pool::tags::forward_shading(), true });
        }

        //-------------------------------------------------------------------------
        void forward_shading_pass::setup(const render_context& context)
        {
            // One-time setup logic (if any) can be added here.
            // For example, initializing resources that persist for the life of this pass.
        }

        //-------------------------------------------------------------------------
        void forward_shading_pass::cleanup(const render_context& context)
        {
            // One-time cleanup logic (if any) can be added here.
            // For example, releasing resources allocated in setup.
        }
    }
}