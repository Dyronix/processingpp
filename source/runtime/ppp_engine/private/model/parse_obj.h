#pragma once

#include "memory/memory_types.h"

#include <string>

namespace ppp
{
    namespace geometry
    {
        class geometry;
    }

    geometry::geometry* parse_obj(geometry::geometry* geom, const transient_scratch_vector<std::string_view>& buffer);
}