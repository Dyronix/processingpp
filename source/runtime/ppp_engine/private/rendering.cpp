#include "rendering.h"
#include "render/render.h"
#include "util/transform_stack.h"
#include "device/device_input.h"

namespace ppp
{
    namespace rendering
    {
        void create_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);

            transform::push();
            transform::translate(glm::vec2(x, y));
        }

        void resize_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);

            // Dequeue the previous one
            transform::pop();
            // Enqueue the new one
            transform::push();
            transform::translate(glm::vec2(x, y));
        }

        void no_canvas()
        {
            transform::pop();

            render::push_scissor_enable(false);
        }

        void wireframe_linewidth(float line_width)
        {
            render::push_wireframe_linewidth(line_width);
        }

        void wireframe_color(float r, float g, float b)
        {
            render::push_wireframe_color({r, g, b, 1.0f});
        }

        void enable_instance_draw_mode()
        {
            render::draw_mode(render::render_draw_mode::INSTANCED);
        }

        void enable_batched_draw_mode()
        {
            render::draw_mode(render::render_draw_mode::BATCHED);
        }
    }
}