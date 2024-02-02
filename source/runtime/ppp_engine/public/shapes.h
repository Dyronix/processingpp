#pragma once

namespace ppp
{
    namespace shapes
    {
        void ellipse(float x, float y, float w, float h, int detail = 25);
        void circle(float x, float y, float r, int detail = 25);
        void line(float x1, float y1, float x2, float y2);
        void point(float x, float y);
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        void rect(float x, float y, float w, float h);
        void square(float x, float y, float s);
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    }
}