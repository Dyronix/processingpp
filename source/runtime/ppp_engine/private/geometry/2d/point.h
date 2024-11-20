#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* extrude_point(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);

        geometry* make_2d_point();
    }
}