#pragma once

namespace ppp
{
    namespace transform
    {
        // Push a new matrix on the stack
        void push();
        // Pop a matrix from the stack
        void pop();

        // Rotates the coordinate system.
        void rotate(float angle);
        void rotate(float axisx, float axisy, float axisz, float angle);
        // Scales the coordinate system.
        void scale(float x, float y);
        void scale(float x, float y, float z);
        // Translates the coordinate system.
        void translate(float x, float y);
        void translate(float x, float y, float z);
    }
}