/**
 * @file color.h
 * @brief Color utilities and drawing state for RGBA colors.
 */
#pragma once

#include <array>

namespace ppp
{
    /**
     * @struct color
     * @brief RGBA color representation.
     */
    struct color
    {
        union
        {
            #pragma warning(push)
            #pragma warning(disable : 4201)
            struct { int red, green, blue, alpha; }; /**< Color channels */
            #pragma warning(pop)
            std::array<int, 4> data;                 /**< Raw channel data */
        };

        /** Set the red channel. */
        void set_red(int r) { red = r; }
        /** Set the green channel. */
        void set_green(int g) { green = g; }
        /** Set the blue channel. */
        void set_blue(int b) { blue = b; }
        /** Set the alpha channel. */
        void set_alpha(int a) { alpha = a; }
    };

    /** @brief Get red component of color. */
    int red(const color& c);
    /** @brief Get green component of color. */
    int green(const color& c);
    /** @brief Get blue component of color. */
    int blue(const color& c);
    /** @brief Get alpha component of color. */
    int alpha(const color& c);

    /** @brief Set background color (grayscale). */
    void background(int grayscale);
    /** @brief Set background color (grayscale + alpha). */
    void background(int grayscale, int alpha);
    /** @brief Set background color (RGBA channels). */
    void background(int r, int g, int b, int a);
    /** @brief Set background color from color struct. */
    void background(const color& c);

    /** @brief Clear canvas with current background. */
    void clear();
    /** @brief Clear canvas with grayscale background. */
    void clear(int grayscale);
    /** @brief Clear canvas with grayscale and alpha. */
    void clear(int grayscale, int alpha);
    /** @brief Clear canvas with RGBA background. */
    void clear(int r, int g, int b, int a);
    /** @brief Clear canvas with color struct background. */
    void clear(const color& c);

    /** @brief Disable filling shapes. */
    void no_fill();
    /** @brief Disable drawing shape outlines. */
    void no_stroke();

    /** @brief Enable filling with grayscale. */
    void fill(int grayscale);
    /** @brief Enable filling with grayscale + alpha. */
    void fill(int grayscale, int alpha);
    /** @brief Enable filling with RGBA. */
    void fill(int r, int g, int b, int a);
    /** @brief Enable filling with color struct. */
    void fill(const color& c);

    /** @brief Enable stroke with grayscale. */
    void stroke(int grayscale);
    /** @brief Enable stroke with grayscale + alpha. */
    void stroke(int grayscale, int alpha);
    /** @brief Enable stroke with RGBA. */
    void stroke(int r, int g, int b, int a);
    /** @brief Enable stroke with color struct. */
    void stroke(const color& c);
    /** @brief Set stroke width. */
    void stroke_weight(float w);

    /** @brief Enable inner stroke with grayscale. */
    void inner_stroke(int grayscale);
    /** @brief Enable inner stroke with grayscale + alpha. */
    void inner_stroke(int grayscale, int alpha);
    /** @brief Enable inner stroke with RGBA. */
    void inner_stroke(int r, int g, int b, int a);
    /** @brief Enable inner stroke with color struct. */
    void inner_stroke(const color& c);
    /** @brief Set inner stroke width. */
    void inner_stroke_weight(float w);

    /**
     * @brief Linearly interpolate between two colors.
     * @param c1 Start color.
     * @param c2 End color.
     * @param t Interpolation factor [0,1].
     * @return Interpolated color.
     */
    color lerp_color(const color& c1, const color& c2, float t);
}
