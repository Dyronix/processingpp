#pragma once

#include <string>
#include <map>

namespace ppp
{
    namespace typography
    {
        using font_id = unsigned int;

        void text_size(unsigned int size);
        void text(const std::string& text, float x, float y);
        void text_font(const font_id& font);

        font_id load_font(const std::string& path, unsigned int size = 16, unsigned int characters_to_load = 128);
    }
}