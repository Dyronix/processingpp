/**
 * @file image.h
 * @brief Create, load, manipulate, and draw images.
 */

#pragma once

#include "color.h"

#include <string>
#include <vector>

namespace ppp
{
    using image_id = unsigned int; /**< Unique image identifier. */

    using pixels_u8 = unsigned char; /**< 8-bit pixel type. */
    using pixels_s32 = unsigned int; /**< 32-bit pixel type. */
    using pixels_u8_ptr = pixels_u8*; /**< Pointer to 8-bit pixel array. */
    using pixels_s32_ptr = pixels_s32*; /**< Pointer to 32-bit pixel array. */

    /**
     * @brief Describe an image resource.
     */
    struct image
    {
        image_id id;    /**< Identifier of the image. */
        int width;      /**< Width in pixels. */
        int height;     /**< Height in pixels. */
        int channels;   /**< Number of color channels. */
    };

    /**
     * @brief Create a blank or data-backed image.
     * @param w Width of image.
     * @param h Height of image.
     * @param c Number of channels.
     * @param data Optional initial pixel data.
     * @return Descriptor for the new image.
     */
    image create(float w, float h, int c, pixels_u8_ptr data);

    /**
     * @brief Rotate pixels clockwise by 90 degrees increments.
     * @param a Pointer to source pixel array.
     * @param w Width of original image.
     * @param h Height of original image.
     * @param n Number of 90 degrees rotations.
     * @return Rotated pixel vector.
     */
    std::vector<pixels_s32> rotate_clockwise(const pixels_s32_ptr a, int w, int h, int n);

    /**
     * @brief Rotate pixels counter-clockwise by 90 degrees increments.
     */
    std::vector<pixels_s32> rotate_counterclockwise(const pixels_s32_ptr a, int w, int h, int n);

    /** @brief Positioning modes for images. */
    enum class image_mode_type : std::uint8_t
    {
        CENTER,
        CORNER
    };

    /**
     * @brief Set image drawing mode (CENTER or CORNER).
     * @param m Drawing mode.
     */
    void image_mode(image_mode_type m);

    /**
     * @brief Load an image file into memory.
     * @param path Filesystem path to the image.
     * @return Descriptor for the loaded image.
     */
    image load(std::string_view path);

    /**
     * @brief Draw a loaded image to the canvas.
     */
    void draw(image_id id, float x, float y, float w, float h);

    /** @brief Disable any active tint. */
    void no_tint();
    /** @brief Apply a grayscale tint. */
    void tint(int g);
    /** @brief Apply a grayscale tint with alpha. */
    void tint(int g, int a);
    /** @brief Apply RGBA tint color. */
    void tint(int r, int g, int b, int a);
    /** @brief Apply tint using a color struct. */
    void tint(const color& c);

    /**
     * @brief Read pixels from the canvas into an array.
     */
    unsigned char* load_pixels(int x, int y, int w, int h);

    /**
     * @brief Read pixels from a loaded image.
     */
    unsigned char* load_pixels(image_id id);

    /**
     * @brief Update a loaded image's pixel data from the canvas.
     */
    void update_pixels(image_id id);

    /**
     * @brief Save raw pixel data to a PNG file.
     */
    void save_pixels(std::string_view name, unsigned char* data, int w, int h, int c);

    /**
     * @brief Save the current canvas pixels to a PNG file.
     */
    void save_pixels(std::string_view name, int w, int h);

    /**
     * @brief Save a loaded image's pixels to a PNG file.
     */
    void save_pixels(std::string_view name, image_id id);

    /**
     * @brief Read raw canvas pixels (RGBA).
     * @return Pointer to pixel data.
     */
    pixels_u8_ptr pixels_as_u8();

    /**
     * @brief Read raw canvas pixels as 32-bit values.
     * @return Pointer to pixel data.
     */
    pixels_s32_ptr pixels_as_u32();
}