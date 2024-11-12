#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* make_cone(bool smooth_normals, bool cap, s32 detail);
    }
}