#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        geometry* make_truncated_cone(bool smooth_normals, f32 in_bottom_radius, f32 in_top_radius, f32 in_height, s32 in_detail_x, s32 in_detail_y, bool top_cap, bool bottom_cap);
    }
}