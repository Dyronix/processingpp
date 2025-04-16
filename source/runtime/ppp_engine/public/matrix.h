#pragma once

#include "vector.h"   // Your existing vector types (base_vec2, base_vec3)

namespace ppp
{
    // 2x2 Matrix: uses 2 columns of type base_vec2
    template<typename T = float>
    struct base_mat2
    {
        base_vec2<T> cols[2];

        // Default constructor initializes to identity matrix:
        // [ 1 0 ]
        // [ 0 1 ]
        base_mat2() : cols{ base_vec2<T>(1, 0),
                       base_vec2<T>(0, 1) } 
        {}

        // Optionally, allow construction with specified columns
        base_mat2(const base_vec2<T>& col0, const base_vec2<T>& col1)
            : cols{ col0, col1 } 
        {}
    };

    // 3x3 Matrix: uses 3 columns of type base_vec3
    template<typename T = float>
    struct base_mat3
    {
        base_vec3<T> cols[3];

        // Default constructor initializes to identity matrix:
        // [ 1 0 0 ]
        // [ 0 1 0 ]
        // [ 0 0 1 ]
        base_mat3() : cols{ base_vec3<T>(1, 0, 0),
                       base_vec3<T>(0, 1, 0),
                       base_vec3<T>(0, 0, 1) } 
        {}

        // Optionally, allow construction with specified columns
        base_mat3(const base_vec3<T>& col0, const base_vec3<T>& col1, const base_vec3<T>& col2)
            : cols{ col0, col1, col2 } 
        {}
    };

    // 4x4 Matrix: uses 4 columns of type vec4
    template<typename T = float>
    struct base_mat4 
    {
        base_vec4<T> cols[4];

        // Default constructor initializes to identity matrix:
        // [ 1 0 0 0 ]
        // [ 0 1 0 0 ]
        // [ 0 0 1 0 ]
        // [ 0 0 0 1 ]
        base_mat4() : cols{ base_vec4<T>(1, 0, 0, 0),
                       base_vec4<T>(0, 1, 0, 0),
                       base_vec4<T>(0, 0, 1, 0),
                       base_vec4<T>(0, 0, 0, 1) } 
        {}

        // Optionally, allow construction with specified columns
        base_mat4(const base_vec4<T>& col0, const base_vec4<T>& col1, const base_vec4<T>& col2, const base_vec4<T>& col3)
            : cols{ col0, col1, col2, col3 } 
        {}
    };

    using mat2f = base_mat2<float>;
    using mat2d = base_mat2<double>;
    using mat2 = mat2f;

    using mat3f = base_mat3<float>;
    using mat3d = base_mat3<double>;
    using mat3 = mat3f;

    using mat4f = base_mat4<float>;
    using mat4d = base_mat4<double>;
    using mat4 = mat4f;

} // namespace ppp