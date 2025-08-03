#include "ray.h"

#include <algorithm>

namespace ppp
{
    bool ray_intersects_aabb(const glm::vec3& ray_origin, const glm::vec3& ray_dir, const glm::vec3& aabb_min, const glm::vec3& aabb_max, float& t_min_out)
    {
        float tmin = -std::numeric_limits<float>::infinity();
        float tmax = std::numeric_limits<float>::infinity();

        for (int i = 0; i < 3; ++i)
        {
            if (ray_dir[i] != 0.0f)
            {
                float t1 = (aabb_min[i] - ray_origin[i]) / ray_dir[i];
                float t2 = (aabb_max[i] - ray_origin[i]) / ray_dir[i];

                if (t1 > t2)
                    std::swap(t1, t2);

                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);

                if (tmin > tmax)
                    return false;
            }
            else
            {
                // Ray is parallel to slab; no hit if origin not within slab
                if (ray_origin[i] < aabb_min[i] || ray_origin[i] > aabb_max[i])
                    return false;
            }
        }

        t_min_out = tmin;
        return true;
    }
}