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
        //struct material
        //{
        //    glm::vec4 diffuse_color;
        //    glm::vec4 ambient_color;
        //    glm::vec4 specular_color;
        //
        //    std::vector<render::texture_id> texture_ids;
        //};

        geometry::geometry* parse_obj(geometry::geometry* geom, const std::vector<std::string> buffer/*, std::unordered_map<std::string, material> materials = {}*/);
    }
}