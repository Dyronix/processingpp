#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

#include <map>

struct FT_FaceRec_;

namespace ppp
{
        struct font_character
        {
            glm::ivec2 size;
            glm::ivec2 bearing;
            u32 advance;
            glm::vec2 uv_start;
            glm::vec2 uv_end;
        };

        struct font_atlas
        {
            u32 texture_id;

            u32 texture_width;
            u32 texture_height;

            std::map<u32, font_character> characters;
        };

        font_atlas make_font_atlas(FT_FaceRec_* face, u32 characters_to_load);
}