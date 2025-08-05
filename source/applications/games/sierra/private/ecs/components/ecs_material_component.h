#pragma once

#include <string>

namespace ppp
{
    namespace ecs
    {
        struct material_component
        {
            std::string_view tag;

            bool enable_depth_test = true;
            bool enable_depth_write = true;
        };
    }
}