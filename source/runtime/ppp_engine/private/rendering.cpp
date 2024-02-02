#include "rendering.h"
#include "render/render.h"
#include "device/device_input.h"

namespace ppp
{
    namespace rendering
    {
        void create_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);

            device::input::push_canvas_enable(true);
            device::input::push_canvas_dimensions(x, y, w, h);
        }

        void resize_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);

            device::input::push_canvas_enable(true);
            device::input::push_canvas_dimensions(x, y, w, h);
        }

        void no_canvas()
        {
            render::push_scissor_enable(false);

            device::input::push_canvas_enable(false);
        }
    }
}