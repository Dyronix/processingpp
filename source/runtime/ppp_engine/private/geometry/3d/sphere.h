#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* make_sphere(bool smooth_normals, s32 detail_x, s32 detail_y);
    }
}