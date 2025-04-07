#include "render/render_pass_unlit_wireframe.h"
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
#include "util/color_ops.h"

#include <algorithm>
#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        static void push_all_wireframe_dependent_uniforms(resources::shader_program shader_program, s32 line_color, f32 line_width)
        {
            shaders::push_uniform(shader_program->id(), string::store_sid("u_wireframe_color"), color::convert_color(line_color));
            opengl::api::instance().line_width(line_width);
        }
        //-------------------------------------------------------------------------
        static void push_all_shape_dependent_uniforms(resources::shader_program shader_program, const glm::mat4& vp)
        {
            shaders::push_uniform(shader_program->id(), string::store_sid("u_view_proj"), vp);
        }

        //-------------------------------------------------------------------------
        unlit_wireframe_pass::unlit_wireframe_pass(const string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags, draw_mode draw_mode)
            :geometry_render_pass("unlit_wireframe"_sid, shader_tag, framebuffer_tag, framebuffer_flags, draw_mode)
        {}
        //-------------------------------------------------------------------------
        unlit_wireframe_pass::~unlit_wireframe_pass() = default;

        //-------------------------------------------------------------------------
        void unlit_wireframe_pass::begin_frame(const render_context& context)
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
        void unlit_wireframe_pass::render(const render_context& context)
        {
            bool batched_shading = batch_rendering_enabled();

            if (batched_shading)
            {
                push_all_wireframe_dependent_uniforms(shader_program(), batch_renderer::wireframe_linecolor(), batch_renderer::wireframe_linewidth());
                shaders::apply_uniforms(shader_program()->id());

                for (auto& [key, batch] : *context.batch_data)
                {
                    batch_renderer::render(batch_render_strategy(), batch.get());
                }
            }
            else
            {
                push_all_wireframe_dependent_uniforms(shader_program(), instance_renderer::wireframe_linecolor(), instance_renderer::wireframe_linewidth());
                shaders::apply_uniforms(shader_program()->id());

                for (auto& [key, instance] : *context.instance_data)
                {
                    instance_renderer::render(instance_render_strategy(), instance.get());
                }
            }
        }

        //-------------------------------------------------------------------------
        void unlit_wireframe_pass::end_frame(const render_context& context)
        {
            // Reset state
            opengl::api::instance().depth_mask(GL_TRUE);
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            framebuffer()->unbind();
        }
    }
}