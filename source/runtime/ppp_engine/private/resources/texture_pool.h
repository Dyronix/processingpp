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

        bool has_image(const std::string& file_path);
        bool has_image(s32 id);

        const Image* image_at_path(const std::string& file_path);
        const Image* image_at_id(s32 id);

        void add_new_image(const Image& image);

        void load_active_pixels(s32 id);
        void update_active_pixels(s32 id);

        unsigned char* active_pixels();
    }
}