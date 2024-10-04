#include "rendering.h"
#include "render/render.h"
#include "render/render_transform.h"
#include "device/device_input.h"

namespace ppp
{
    namespace rendering
    {
        void create_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);

            render::transform::push();
            render::transform::translate(glm::vec2(x, y));
        }

        void resize_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);

            // Dequeue the previous one
            render::transform::pop();
            // Enqueue the new one
            render::transform::push();
            render::transform::translate(glm::vec2(x, y));
        }

        void no_canvas()
        {
            render::transform::pop();

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
    }
}