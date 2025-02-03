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

        struct image
        {
            image_id id = 0;

            int width = 0;
            int height = 0;
            int channels = 0;
        };

        enum class image_mode_type
        {
            CENTER,
            CORNER
        };

        void image_mode(image_mode_type mode);

        unsigned char* load_pixels(int x, int y, int width, int height);
        unsigned char* load_pixels(image_id id);

        void update_pixels(image_id id);

        void save_pixels(std::string_view output_name, unsigned char* data, int width, int height, int channels);
        void save_pixels(std::string_view output_name, int width, int height);
        void save_pixels(std::string_view output_name, image_id id);

        pixels_u8_ptr pixels_as_u8();
        pixels_s32_ptr pixels_as_u32();

        image load(std::string_view image);
        image create(float width, float height, int channels, pixels_u8_ptr data);

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