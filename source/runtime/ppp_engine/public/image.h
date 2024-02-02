#pragma once

#include "color.h"

#include <string>

namespace ppp
{
    namespace image
    {
        using image_id = unsigned int;

        image_id load(const std::string& image);

        void draw(image_id image_id, float x, float y, float width, float height);

        void no_tint();

        void tint(int grayscale);
        void tint(int grayscale, int alpha);
        void tint(int r, int g, int b, int a);
        void tint(const color::Color& c);
    }
}