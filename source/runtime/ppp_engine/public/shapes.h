#pragma once

#include "types.h"

namespace ppp
{
    namespace shapes
    {
        void ellipse(f32 x, f32 y, f32 w, f32 h, s32 detail = 25);
        void circle(f32 x, f32 y, f32 r, s32 detail = 25);
        void line(f32 x1, f32 y1, f32 x2, f32 y2);
        void point(f32 x, f32 y);
        void polygon(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4);
        void rect(f32 x, f32 y, f32 w, f32 h);
        void square(f32 x, f32 y, f32 s);
        void triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3);
    }
}