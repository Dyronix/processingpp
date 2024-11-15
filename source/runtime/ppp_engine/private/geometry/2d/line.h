#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* extrude_line(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);

        geometry* make_line(f32 x1, f32 y1, f32 x2, f32 y2);
    }
}