#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* extrude_rectangle(const glm::mat4& world, const geometry* in_geom, f32 extrusion_width);

        geometry* make_rectangle();
    }
}