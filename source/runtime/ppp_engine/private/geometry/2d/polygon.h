#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* extrude_polygon(const glm::mat4& world, const geometry* in_geom, f32 extrusion_width);

        geometry* make_polygon(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4);
    }
}