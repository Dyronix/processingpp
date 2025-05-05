/**
 * @file vector.h
 * @brief Defines 2D, 3D, and 4D vector type aliases.
 */
#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ppp
{
    using vec2i = glm::vec<2, int, glm::defaultp>;
    using vec2f = glm::vec<2, float, glm::defaultp>;
    using vec2d = glm::vec<2, double, glm::defaultp>;
    using vec2 = vec2f;

    using vec3i = glm::vec<3, int, glm::defaultp>;
    using vec3f = glm::vec<3, float, glm::defaultp>;
    using vec3d = glm::vec<3, double, glm::defaultp>;
    using vec3 = vec3f;

    using vec4i = glm::vec<4, int, glm::defaultp>;
    using vec4f = glm::vec<4, float, glm::defaultp>;
    using vec4d = glm::vec<4, double, glm::defaultp>;
    using vec4 = vec4f;

} // namespace ppp