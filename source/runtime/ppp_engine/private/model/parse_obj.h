#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace ppp
{
    namespace geometry
    {
        class geometry;
    }

    namespace model
    {
        geometry::geometry* parse_obj(geometry::geometry* geom, const fileio_vector<std::string_view>& buffer);
    }
}