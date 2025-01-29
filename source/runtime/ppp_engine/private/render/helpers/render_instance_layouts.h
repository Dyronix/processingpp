#pragma once

#include "render/render_item.h"

#include "util/types.h"

#include <glm/glm.hpp>

#include <array>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        struct world_color_format
        {
            glm::mat4 world;
            glm::vec4 color;
        };
        //-------------------------------------------------------------------------
        struct world_color_matid_format
        {
            glm::mat4 world;
            glm::vec4 color;
            s32       material_idx;
        };

        const std::array<attribute_layout, 2>& color_world_layout();
        const std::array<attribute_layout, 3>& color_world_matid_layout();
    }
}