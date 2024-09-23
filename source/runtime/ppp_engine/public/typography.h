#pragma once

#include <string>

namespace ppp
{
    namespace typography
    {
        using font_id = unsigned int;

        struct Font
        {
            font_id id = 0;
        };

        void text_size(float size);
        void text(const std::string& text, float x, float y);
        void text_font(const Font& font);

        Font load_font(const std::string& font);
    }
}