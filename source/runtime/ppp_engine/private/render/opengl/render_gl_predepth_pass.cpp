#include "render/render_predepth_pass.h"
#include "render/render_batch_renderer.h"
#include "render/render_batch_data_table.h"
#include "render/render_instance_renderer.h"
#include "render/render_instance_data_table.h"
#include "render/render_context.h"
#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/shader_pool.h"
#include "resources/material.h"

#include "camera/camera_context.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        static void push_shape_uniforms(resources::shader_program shader_program, const glm::mat4& vp)
        {
            shaders::push_uniform(shader_program->id(), string::store_sid("u_view_proj"), vp);
        }

        //-------------------------------------------------------------------------
        predepth_pass::predepth_pass(string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags)
            :render_pass(shader_tag, framebuffer_tag, framebuffer_flags)
        {}

        //-------------------------------------------------------------------------
        predepth_pass::~predepth_pass() = default;

        //-------------------------------------------------------------------------
        void predepth_pass::begin_frame(const render_context& context)
        {
            assert(context && "Invalid render context");

            framebuffer()->bind();

            // Configure OpenGL state
            opengl::api::instance().disable(GL_BLEND);

            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().cull_face(GL_BACK);

            opengl::api::instance().enable(GL_DEPTH_TEST);
            opengl::api::instance().depth_mask(GL_TRUE); // Allow depth write
            opengl::api::instance().depth_func(GL_LESS);

            opengl::api::instance().color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes

            opengl::api::instance().viewport(0, 0, framebuffer()->width(), framebuffer()->height());
            opengl::api::instance().clear_depth(1.0f);
            opengl::api::instance().clear(GL_DEPTH_BUFFER_BIT);

            opengl::api::instance().polygon_mode(GL_FRONT_AND_BACK, GL_FILL);

            // Bind shader
            opengl::api::instance().use_program(shader_program()->id());

            // Apply shape uniforms
            const glm::mat4& cam_active_p = context.camera_context->mat_proj_active;
            const glm::mat4& cam_active_v = context.camera_context->mat_view_active;

            const glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            push_shape_uniforms(shader_program(), cam_active_vp);
        }

        //-------------------------------------------------------------------------
        void predepth_pass::render(const render_context& context)
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
        void predepth_pass::end_frame(const render_context& context)
        {
            // Reset state
            opengl::api::instance().color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Re-enable color writes
            opengl::api::instance().use_program(0);

            // Unbind pass framebuffer
            framebuffer()->unbind();
        }
    }
}
