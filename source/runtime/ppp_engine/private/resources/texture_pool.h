#pragma once


#include "util/types.h"
#include "string/string_id.h"

#include <string>

namespace ppp
{
    namespace texture_pool
    {
        struct image
        {
            string::string_id   file_path = string::string_id::create_invalid();

            s32                 image_id = -1;

            s32			        width = -1;
            s32			        height = -1;
            s32			        channels = -1;

            u8*                 data = nullptr;
        };

        bool initialize();
        void terminate();

        bool has_image(string::string_id file_path);
        bool has_image(s32 id);

        const image* image_at_path(string::string_id file_path);
        const image* image_at_id(s32 id);

        void add_new_image(const image& image);

        u8* load_active_pixels(s32 id);
        u8* load_active_pixels(s32 x, s32 y, s32 width, s32 height, s32 channels);

        void update_active_pixels(s32 id);

        u8* active_pixels();
    }
}