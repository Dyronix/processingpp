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

        bool has_geometry(const std::string& geometry_id);
        bool has_geometry(u64 geometry_id);

        geometry::geometry* add_new_geometry(geometry::geometry&& geom);

        geometry::geometry* get_geometry(const std::string& geometry_id);
        geometry::geometry* get_geometry(u64 geometry_id);
    }
}