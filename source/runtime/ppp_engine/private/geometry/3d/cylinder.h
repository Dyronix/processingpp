#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* make_cylinder(bool smooth_normals, bool top, bool bottom, s32 detail);
    }
}