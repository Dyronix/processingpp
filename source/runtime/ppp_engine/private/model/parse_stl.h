#pragma once

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        class geometry;
    }

    namespace model
    {
        void parse_stl(geometry::geometry* geom, const std::vector<u8>& buffer);
    }
}