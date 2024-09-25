#pragma once

#include "util/types.h"

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
            std::string file_path = {};

            typography::FontAtlas atlas;
        };

        bool initialize();
        void terminate();

        bool has_font(const std::string& file_path);
        bool has_font(u32 id);

        const Font* font_at_path(const std::string& file_path);
        const Font* font_at_id(u32 id);

        void load_active_font(u32 id);

        void add_new_font(const Font& font);

        const Font* active_font();
    }
}