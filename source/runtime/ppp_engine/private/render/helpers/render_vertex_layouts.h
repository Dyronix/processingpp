#pragma once

#include "render/render_item.h"
#include "render/render_types.h"

#include "util/types.h"

#include <glm/glm.hpp>

#include <array>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        struct pos_format
        {
            glm::vec3 position;
        };
        //-------------------------------------------------------------------------
        struct pos_norm_format
        {
            glm::vec3 position;
            glm::vec3 normal;
        };
        //-------------------------------------------------------------------------
        struct pos_norm_color_format
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec4 color;
        };
        //-------------------------------------------------------------------------
        struct pos_tex_format
        {
            glm::vec3 position;
            glm::vec2 texcoord;
        };
        //-------------------------------------------------------------------------
        struct pos_tex_norm_format
        {
            glm::vec3 position;
            glm::vec2 texcoord;
            glm::vec3 normal;
        };
        //-------------------------------------------------------------------------
        struct pos_col_format
        {
            glm::vec3 position;
            glm::vec4 color;
        };
        //-------------------------------------------------------------------------
        struct pos_tex_col_format
        {
            glm::vec3 position;
            glm::vec2 texcoord;
            glm::vec4 color;
            s32       material_idx;
        };
        //-------------------------------------------------------------------------
        struct pos_tex_col_norm_format
        {
            glm::vec3 position;
            glm::vec2 texcoord;
            glm::vec3 normal;
            glm::vec4 color;
            s32       material_idx;
        };

        const std::array<attribute_layout, 1>& pos_layout();
        const std::array<attribute_layout, 2>& pos_norm_layout();
        const std::array<attribute_layout, 3>& pos_norm_col_layout();
        const std::array<attribute_layout, 2>& pos_col_layout();
        const std::array<attribute_layout, 2>& pos_tex_layout();
        const std::array<attribute_layout, 3>& pos_tex_norm_layout();
        const std::array<attribute_layout, 4>& pos_tex_col_layout();
        const std::array<attribute_layout, 5>& pos_tex_col_norm_layout();

        const attribute_layout* fill_user_layout(vertex_type type);
        u64 fill_user_layout_count(vertex_type type);
    }
}