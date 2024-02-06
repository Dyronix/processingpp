#pragma once

#include "color.h"

#include <string>

namespace ppp
{
    namespace image
    {
        using image_id = unsigned int;

        enum class ImageMode
        {
            CENTER,
            CORNER
        };

        void image_mode(ImageMode mode);

        void load_pixels(image_id id);
        void update_pixels(image_id id);

        unsigned char* pixels_as_u8();
        unsigned int* pixels_as_u32();

        image_id load(const std::string& image);
        image_id create(float width, float height, int channels, unsigned char* data);

        void draw(image_id image_id, float x, float y, float width, float height);

        void no_tint();

        void tint(int grayscale);
        void tint(int grayscale, int alpha);
        void tint(int r, int g, int b, int a);
        void tint(const color::Color& c);
    }
}