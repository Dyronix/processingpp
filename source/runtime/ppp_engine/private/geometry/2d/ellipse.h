#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* extrude_ellipse(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);

        geometry* make_ellipse(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 detail = 25);
    }
}