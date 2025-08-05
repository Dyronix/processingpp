#include "render/render_pass_ui_wireframe.h"
#include "render/render_batch_renderer.h"
#include "render/render_batch_data_table.h"
#include "render/render_instance_renderer.h"
#include "render/render_context.h"
#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/shader_pool.h"

#include "camera/camera_context.h"
#include "camera/camera_manager.h"

#include "util/color_ops.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        static void push_all_wireframe_dependent_uniforms(resources::shader_program shader_program, s32 line_color, f32 line_width)
        {
            shaders::push_uniform(shader_program->id(), string::store_sid("u_wireframe"), 1);
            shaders::push_uniform(shader_program->id(), string::store_sid("u_wireframe_color"), color::convert_color(line_color));
            opengl::api::instance().line_width(line_width);
        }
        //-------------------------------------------------------------------------
        static void push_all_shape_dependent_uniforms(resources::shader_program shader_program, const glm::mat4& vp)
        {
            shaders::push_uniform(shader_program->id(), string::store_sid("u_view_proj"), vp);
        }

        //-------------------------------------------------------------------------
        ui_wireframe_pass::ui_wireframe_pass(string::string_id shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags, draw_mode draw_mode)
            :geometry_render_pass("ui_wireframe"_sid, shader_tag, framebuffer_tag, framebuffer_flags, draw_mode)
        {
        }
        //-------------------------------------------------------------------------
        ui_wireframe_pass::~ui_wireframe_pass() = default;

        //-------------------------------------------------------------------------
        void ui_wireframe_pass::begin_frame(const render_context& context)
        {
            framebuffer()->bind();

            // Configure OpenGL state.
            opengl::api::instance().disable(GL_BLEND);

            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().cull_face(GL_BACK);

            opengl::api::instance().disable(GL_DEPTH_TEST);

            opengl::api::instance().viewport(0, 0, framebuffer()->width(), framebuffer()->height());
            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_LINE);

            // Bind the pass shader
            opengl::api::instance().use_program(shader_program()->id());

            // Apply shape uniforms
            const glm::mat4& cam_active_p = camera_manager::get_proj(camera_manager::tags::ui());
            const glm::mat4& cam_active_v = camera_manager::get_view(camera_manager::tags::ui());

            const glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            push_all_shape_dependent_uniforms(shader_program(), cam_active_vp);
        }

        //-------------------------------------------------------------------------
        void ui_wireframe_pass::render(const render_context& context)
        {
            bool batched_shading = batch_rendering_enabled();

            if (batched_shading)
            {
                push_all_wireframe_dependent_uniforms(shader_program(), batch_renderer::wireframe_linecolor(), batch_renderer::wireframe_linewidth());
                shaders::apply_uniforms(shader_program()->id());

                for (auto& [key, batch] : *context.ui_batch_data)
                {
                    batch_renderer::render(batch_render_strategy(), batch.get());
                }
            }
            else
            {
                push_all_wireframe_dependent_uniforms(shader_program(), instance_renderer::wireframe_linecolor(), instance_renderer::wireframe_linewidth());
                shaders::apply_uniforms(shader_program()->id());

                for (auto& [key, instance] : *context.ui_instance_data)
                {
                    instance_renderer::render(instance_render_strategy(), instance.get());
                }
            }
        }

        //-------------------------------------------------------------------------
        void ui_wireframe_pass::end_frame(const render_context& context)
        {
            // Reset state
            opengl::api::instance().depth_mask(GL_TRUE);
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            framebuffer()->unbind();
        }
    }
}