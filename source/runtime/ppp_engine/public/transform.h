#pragma once

namespace ppp
{
    namespace transform
    {
        void push();
        void pop();

        void rotate(float angle);
        void rotate(float axisx, float axisy, float axisz, float angle);
        void scale(float x, float y);
        void scale(float x, float y, float z);
        void translate(float x, float y);
        void translate(float x, float y, float z);
    }
}