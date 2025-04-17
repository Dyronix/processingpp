/**
 * @file matrix.h
 * @brief Defines matrix type aliases for 2x2, 3x3, and 4x4 matrices.
 */
#pragma once

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat2x2.hpp>

namespace ppp
{
    using mat2f = glm::mat<2, 2, float, glm::defaultp>;
    using mat2d = glm::mat<2, 2, double, glm::defaultp>;
    using mat2 = mat2f;

    using mat3f = glm::mat<3, 3, float, glm::defaultp>;
    using mat3d = glm::mat<3, 3, double, glm::defaultp>;
    using mat3 = mat3f;

    using mat4f = glm::mat<4, 4, float, glm::defaultp>;
    using mat4d = glm::mat<4, 4, double, glm::defaultp>;
    using mat4 = mat4f;

} // namespace ppp