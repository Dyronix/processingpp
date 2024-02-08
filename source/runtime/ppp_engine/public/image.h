#pragma once

#include "color.h"

#include <string>
#include <vector>

namespace ppp
{
    namespace image
    {
        using image_id = unsigned int;

        using pixels_u8 = unsigned char;
        using pixels_u8_ptr = pixels_u8*;
        using pixels_s32 = unsigned int;
        using pixels_s32_ptr = pixels_s32*;

        struct Image
        {
            image_id id = 0;

            int width = 0;
            int height = 0;
            int channels = 0;
        };

        enum class ImageMode
        {
            CENTER,
            CORNER
        };

        void image_mode(ImageMode mode);

        void load_pixels(image_id id);
        void update_pixels(image_id id);

        pixels_u8_ptr pixels_as_u8();
        pixels_s32_ptr pixels_as_u32();

        Image load(const std::string& image);
        Image create(float width, float height, int channels, pixels_u8_ptr data);

        void draw(image_id image_id, float x, float y, float width, float height);

        void no_tint();

        void tint(int grayscale);
        void tint(int grayscale, int alpha);
        void tint(int r, int g, int b, int a);
        void tint(const color::Color& c);

        std::vector<pixels_s32> rotate_clockwise(const pixels_s32_ptr arr, int width, int height, int num_rotations);
        std::vector<pixels_s32> rotate_counterclockwise(const pixels_s32_ptr arr, int width, int height, int num_rotations);
    }
}