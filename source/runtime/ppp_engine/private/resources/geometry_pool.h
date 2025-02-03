#pragma once

#include "geometry/geometry.h"

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace geometry_pool
    {
        bool initialize();
        void terminate();

        bool has_geometry(std::string_view geometry_id);
        bool has_geometry(u64 geometry_id);

        geometry::geometry* add_new_geometry(geometry::geometry&& geom);

        geometry::geometry* get_geometry(std::string_view geometry_id);
        geometry::geometry* get_geometry(u64 geometry_id);
    }
}