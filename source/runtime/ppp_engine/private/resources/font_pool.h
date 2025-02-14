#pragma once

#include "util/types.h"
#include "memory/memory_types.h"
#include "resources/font_atlas.h"
#include "string/string_id.h"

#include <glm/glm.hpp>

#include <string>
#include <map>

namespace ppp
{
    namespace font_pool
    {
        struct font
        {
            string::string_id file_path = string::string_id::create_invalid();
            u32 size = -1;
            typography::font_atlas atlas;
        };

        bool initialize();
        void terminate();

        bool has_font(string::string_id file_path);
        bool has_font(u32 id);

        const font* font_at_path(string::string_id file_path);
        const font* font_at_id(u32 id);

        void load_active_font(u32 id);

        void add_new_font(const font& font);

        const font* active_font();
    }
}