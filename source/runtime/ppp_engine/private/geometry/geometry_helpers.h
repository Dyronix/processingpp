#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace geometry
    {
        std::vector<glm::vec3> compute_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count, bool smooth_normals);
    }
}