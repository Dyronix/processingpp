#pragma once

#include "vector.h"
#include "matrix.h"

namespace ppp
{
    // Push a new matrix on the stack
    void push();
    // Pop a matrix from the stack
    void pop();
    // Retrieve the active transform matrix
    mat4 active_transform();

    // Rotates the coordinate system.
    void rotate(float angle);
    void rotate(float axisx, float axisy, float axisz, float angle);
    void rotate(const vec3& axis, float angle);

    // Scales the coordinate system.
    void scale(float x, float y);
    void scale(float x, float y, float z);
    void scale(const vec2& scale);
    void scale(const vec3& scale);
    // Translates the coordinate system.
    void translate(float x, float y);
    void translate(float x, float y, float z);
    void translate(const vec2& trans);
    void translate(const vec3& trans);
}