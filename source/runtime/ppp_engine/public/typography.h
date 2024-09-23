#pragma once

#include <string>
#include <map>

namespace ppp
{
    namespace typography
    {
        using font_id = unsigned long long;

        void text_size(float size);
        void text(const std::string& text, float x, float y);
        void text_font(const font_id& font);

        font_id load_font(const std::string& path, unsigned int characters_to_load = 128);
    }
}