#include "rendering.h"
#include "render/render.h"

namespace ppp
{
    namespace rendering
    {
        void create_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);
        }

        void resize_canvas(float x, float y, float w, float h)
        {
            render::push_scissor_enable(true);
            render::push_scissor(x, y, w, h);
        }

        void no_canvas()
        {
            render::push_scissor_enable(false);
        }
    }
}