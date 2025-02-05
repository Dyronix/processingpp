#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include "resources/font_atlas.h"

#include <glm/glm.hpp>

#include <string>
#include <map>

namespace ppp
{
    namespace font_pool
    {
        struct Font
        {
            pool_string file_path = {};
            u32 size = -1;
            typography::FontAtlas atlas;
        };

        bool initialize();
        void terminate();

        bool has_font(std::string_view file_path);
        bool has_font(u32 id);

        const Font* font_at_path(std::string_view file_path);
        const Font* font_at_id(u32 id);

        void load_active_font(u32 id);

        void add_new_font(const Font& font);

        const Font* active_font();
    }
}