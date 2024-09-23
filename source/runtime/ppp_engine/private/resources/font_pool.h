#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace font_pool
    {
        struct Font
        {
            std::string     file_path = {};

            s32             font_id = -1;
        };

        bool initialize();
        void terminate();

        bool has_font(const std::string& file_path);
        bool has_font(s32 id);

        const Font* font_at_path(const std::string& file_path);
        const Font* font_at_id(s32 id);

        void add_new_font(const Font& font);
    }
}