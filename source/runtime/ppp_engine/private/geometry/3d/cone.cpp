#include "geometry/3d/cone.h"
#include "geometry/3d/truncated_cone.h"

#include "resources/geometry_pool.h"

#include "constants.h"

#include <array>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        geometry* make_cone(bool smooth_normals, bool cap, s32 detail)
        {
            const f32 bottom_radius = 1.0f;
            const f32 top_radius = 0.0f;

            const f32 height = 1.0f;

            const s32 detail_x = detail;
            const s32 detail_y = 1;

            return make_truncated_cone(smooth_normals, bottom_radius, top_radius, height, detail_x, detail_y, false, cap);
        }
    }
}