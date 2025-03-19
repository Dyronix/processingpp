#pragma once

#include "color.h"

#include <string>
#include <vector>

namespace ppp
{
    namespace image
    {
        // A unique identifier to describe each image
        using image_id = unsigned int;

        // Name aliases for each type of pixel array
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

        // Creation
        // Creates a new image object.
        image create(float width, float height, int channels, pixels_u8_ptr data);

        // Utilities
        // Rotate the pixel array clockwise
        std::vector<pixels_s32> rotate_clockwise(const pixels_s32_ptr arr, int width, int height, int num_rotations);
        // Rotate the pixel array counter clockwise
        std::vector<pixels_s32> rotate_counterclockwise(const pixels_s32_ptr arr, int width, int height, int num_rotations);

        // Attributes
        // The drawing mode on how a particular image is drawn
        enum class image_mode_type
        {
            CENTER,
            CORNER
        };

        // Changes where images are drawn
        void image_mode(image_mode_type mode);

        // Loading & Displaying
        // Loads an image to create a p5.Image object.
        image load(std::string_view image);
        // Draws an image to the canvas.
        void draw(image_id image_id, float x, float y, float width, float height);
        // Removes the current tint set by tint().
        void no_tint();
        // Tints images using a color.
        void tint(int grayscale);
        void tint(int grayscale, int alpha);
        void tint(int r, int g, int b, int a);
        void tint(const color::color& c);

        // Pixels
        // Loads the current value of each pixel on the canvas into the pixels array.
        unsigned char* load_pixels(int x, int y, int width, int height);
        unsigned char* load_pixels(image_id id);
        // Updates the canvas with the RGBA values in the pixels array.
        void update_pixels(image_id id);
        // Write active pixels to disk
        void save_pixels(std::string_view output_name, unsigned char* data, int width, int height, int channels);
        void save_pixels(std::string_view output_name, int width, int height);
        void save_pixels(std::string_view output_name, image_id id);
        // An array containing the color of each pixel on the canvas.
        pixels_u8_ptr pixels_as_u8();
        pixels_s32_ptr pixels_as_u32();
    }
}