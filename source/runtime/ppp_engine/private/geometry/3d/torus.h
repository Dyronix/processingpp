#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* make_torus(bool smooth_normals, f32 radius, f32 tube_radius, s32 detail_x, s32 detail_y);
    }
}