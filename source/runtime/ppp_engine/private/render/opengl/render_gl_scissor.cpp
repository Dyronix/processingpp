#include "render/render_scissor.h"

namespace ppp
{
    namespace render
    {
        render_scissor g_scissor = {};

        //-------------------------------------------------------------------------
        void push_scissor(s32 x, s32 y, s32 width, s32 height)
        {
            g_scissor.x = x;
            g_scissor.y = y;
            g_scissor.width = width;
            g_scissor.height = height;
        }

        //-------------------------------------------------------------------------
        void push_scissor_enable(bool enable)
        {
            g_scissor.enable = enable;
        }

        //-------------------------------------------------------------------------
        bool scissor_rect_enabled()
        {
            return g_scissor.enable;
        }

        //-------------------------------------------------------------------------
        const render_scissor* scissor_rect()
        {
            return &g_scissor;
        }
    }
}