#pragma once

#include "util/types.h"

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
        geometry::geometry* parse_obj(geometry::geometry* geom, const std::vector<std::string> buffer);
    }
}