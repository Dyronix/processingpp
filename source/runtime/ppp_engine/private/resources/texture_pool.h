#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace texture_pool
    {
        struct Image
        {
            std::string     file_path = {};

            s32             image_id = -1;

            s32			    width = -1;
            s32			    height = -1;
            s32			    channels = -1;

            u8*             data;
        };

        bool initialize();
        void terminate();

        bool has_image(std::string_view file_path);
        bool has_image(s32 id);

        const Image* image_at_path(std::string_view file_path);
        const Image* image_at_id(s32 id);

        void add_new_image(const Image& image);

        u8* load_active_pixels(s32 id);
        u8* load_active_pixels(s32 x, s32 y, s32 width, s32 height, s32 channels);

        void update_active_pixels(s32 id);

        unsigned char* active_pixels();
    }
}