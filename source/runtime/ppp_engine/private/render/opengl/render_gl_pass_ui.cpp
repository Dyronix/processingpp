#include "render/render_pass_ui.h"
#include "render/render_batch_renderer.h"
#include "render/render_batch_data_table.h"
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
#include "util/color_ops.h"

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
        ui_pass::ui_pass(const tag<unlit::font> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags)
            :geometry_render_pass("ui"_sid, decltype(shader_tag)::batched(), framebuffer_tag, framebuffer_flags, draw_mode::BATCHED)
        {}
        //-------------------------------------------------------------------------
        ui_pass::~ui_pass() = default;

        //-------------------------------------------------------------------------
        void ui_pass::begin_frame(const render_context& context)
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
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_LINE);

            // Bind the pass shader
            opengl::api::instance().use_program(shader_program()->id());

            // Apply shape uniforms
            const glm::mat4& cam_active_p = context.camera_context->mat_proj_active;
            const glm::mat4& cam_active_v = context.camera_context->mat_view_active;

            const glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            push_all_shape_dependent_uniforms(shader_program(), cam_active_vp);
        }

        //-------------------------------------------------------------------------
        void ui_pass::render(const render_context& context)
        {
            shaders::apply_uniforms(shader_program()->id());

            batch_renderer::render(batch_render_strategy(), context.font_batch_data);
        }

        //-------------------------------------------------------------------------
        void ui_pass::end_frame(const render_context& context)
        {
            // Reset state
            opengl::api::instance().disable(GL_BLEND);
            opengl::api::instance().depth_mask(GL_TRUE);
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            framebuffer()->unbind();
        }
    }
}