/**
 * @file ray.h
 * @brief Ray structure that holds origin and direction of a ray.
 */

#pragma once

#include "vector.h"

namespace ppp
{
    struct ray
    {
        vec3 origin;
        vec3 dir;
    };

    bool ray_intersects_aabb(const glm::vec3& ray_origin, const glm::vec3& ray_dir, const glm::vec3& aabb_min, const glm::vec3& aabb_max, float& t_min_out);
}