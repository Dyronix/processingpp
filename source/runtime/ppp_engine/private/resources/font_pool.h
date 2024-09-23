#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

#include <string>
#include <map>

namespace ppp
{
    namespace font_pool
    {
        struct Character
        {
            u32         texture_id;  // ID handle of the glyph texture
            glm::ivec2  size;        // Size of glyph
            glm::ivec2  bearing;     // Offset from baseline to left/top of glyph
            u32         advance;     // Offset to advance to next glyph
        };

        using characcter_map = std::map<s8, Character>;

        struct Font
        {
            std::string file_path = {};

            u64 font_id = -1;

            std::map<s8, Character> characters;
        };

        bool initialize();
        void terminate();

        bool has_font(const std::string& file_path);
        bool has_font(u64 id);

        const Font* font_at_path(const std::string& file_path);
        const Font* font_at_id(u64 id);

        void load_active_font(u64 id);

        void add_new_font(const Font& font);

        const Font* active_font();
    }
}