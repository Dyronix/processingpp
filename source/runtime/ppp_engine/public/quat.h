/**
 * @file quat.h
 * @brief Defines quaternion type aliases for float and double precision quaternions.
 */
#pragma once

#include <glm/gtc/quaternion.hpp>

namespace ppp
{
    using quatf = glm::qua<float, glm::defaultp>;
    using quatd = glm::qua<double, glm::defaultp>;
    using quat = quatf;

} // namespace ppp