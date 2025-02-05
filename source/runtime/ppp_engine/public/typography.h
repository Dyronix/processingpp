#pragma once

#include <string>
#include <map>

namespace ppp
{
    namespace typography
    {
        using font_id = unsigned int;

        enum class text_mode_type
        {
            CENTER,
            CORNER
        };

        void text_size(unsigned int size);
        void text(std::string_view text, float x, float y);
        void text_font(const font_id& font);
        void text_mode(text_mode_type mode);

        font_id load_font(std::string_view path, unsigned int size = 16, unsigned int characters_to_load = 128);
    }
}