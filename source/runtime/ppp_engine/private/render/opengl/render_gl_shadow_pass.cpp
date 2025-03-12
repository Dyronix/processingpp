#include "render/render_shadow_pass.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"
#include "render/render_context.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"
#include "resources/lights_pool.h"
#include "resources/shader_pool.h"

#include "camera/camera_context.h"

#include "util/log.h"

#include <algorithm>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        shadow_pass::~shadow_pass() = default;

        //-------------------------------------------------------------------------
        void shadow_pass::begin_frame(const render_context& context)
        {
            assert(context && "Invalid render context");

            auto framebuffer = framebuffer_pool::get(framebuffer_pool::tags::shadow_map(), framebuffer_flags::SAMPLED_DEPTH);

            framebuffer->bind();

            // Set viewport and clear the framebuffer.
            opengl::api::instance().viewport(0, 0, framebuffer->width(), framebuffer->height());
            opengl::api::instance().clear(GL_DEPTH_BUFFER_BIT);
            opengl::api::instance().clear_depth(1.0);
        }

        //-------------------------------------------------------------------------
        void shadow_pass::render(const render_context& context)
        {
            assert(lights_pool::directional_lights().size() == 1);

            auto framebuffer = framebuffer_pool::get(framebuffer_pool::tags::shadow_map(), framebuffer_flags::SAMPLED_DEPTH);

            auto& dir_light = lights_pool::directional_lights()[0];

            // Retrieve camera parameters.
            glm::vec3 light_pos_active = -dir_light.direction * 100.0f;
            glm::vec3 light_tar_active = glm::vec3(0.0f);

            // Configure OpenGL state.
            opengl::api::instance().disable(GL_BLEND);

            opengl::api::instance().enable(GL_CULL_FACE);
            opengl::api::instance().enable(GL_DEPTH_TEST);

            opengl::api::instance().blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            opengl::api::instance().cull_face(GL_BACK);
            opengl::api::instance().depth_func(GL_LESS);

            // Render batched and instanced geometry.
            f32 near_plane = 1.0f, far_plane = 1000.0f;
            const glm::mat4 light_active_p = glm::ortho(-framebuffer->width() / 2.0f, framebuffer->width() / 2.0f, -framebuffer->height() / 2.0f, framebuffer->height() / 2.0f, near_plane, far_plane);;
            const glm::mat4 light_active_v = glm::lookAt(light_pos_active, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

            glm::mat4 light_active_vp = light_active_p * light_active_v;

            for (auto& pair : *context.batch_renderers)
            {
                pair.second->user_shader_program(shader_pool::tags::unlit::shadow());
                pair.second->render(light_pos_active, light_tar_active, light_active_vp);
            }

            for (auto& pair : *context.instance_renderers)
            {
                pair.second->user_shader_program(shader_pool::tags::unlit::shadow());
                pair.second->render(light_pos_active, light_tar_active, light_active_vp);
            }
        }

        //-------------------------------------------------------------------------
        void shadow_pass::end_frame(const render_context& context)
        {
            framebuffer_pool::unbind(framebuffer_pool::tags::shadow_map());
        }

        //-------------------------------------------------------------------------
        bool shadow_pass::should_render() const
        {
            auto& dir_lights = lights_pool::directional_lights();
            auto num = std::count_if(std::cbegin(dir_lights), std::end(dir_lights),
                [](const lights_pool::directional_light& light)
            {
                return light.cast_shadows;
            });

            return num > 0;
        }
    }
}