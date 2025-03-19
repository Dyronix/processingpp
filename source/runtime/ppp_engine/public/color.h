#pragma once

#include <glm/glm.hpp>

#include <array>

namespace ppp
{
    namespace color
    {
        // Creating & Reading
        struct color
        {
            union
            {
                // Disabling this warning because it has to be nameless.
                #pragma warning(push)
                #pragma warning(disable : 4201)
                struct
                {
                    int red, green, blue, alpha;
                };
                #pragma warning(pop)

                std::array<int, 4> data;
            };

            void set_red(int r) { red = r; }
            void set_green(int g) { green = g; }
            void set_blue(int b) { blue = b; }
            void set_alpha(int a) { alpha = a; }
        };

        // Gets the red value of a color.
        int red(const color& c);
        // Gets the green value of a color.
        int green(const color& c);
        // Gets the blue value of a color.
        int blue(const color& c);
        // Gets the alpha value of a color.
        int alpha(const color& c);

        // Setting
        // Sets the color used for the background of the canvas.
        void background(int grayscale);
        void background(int grayscale, int alpha);
        void background(int r, int g, int b, int a);
        void background(const color& c);
        // Clears the pixels on the canvas.
        void clear();
        void clear(int grayscale);
        void clear(int grayscale, int alpha);
        void clear(int r, int g, int b, int a);
        void clear(const color& c);
        // Disables setting the fill color for shapes.
        void no_fill();
        // Disables drawing points, lines, and the outlines of shapes.
        void no_stroke();
        // Sets the color used to fill shapes.
        void fill(int grayscale);
        void fill(int grayscale, int alpha);
        void fill(int r, int g, int b, int a);
        void fill(const color& c);
        // Sets the color used to draw the outlines of shapes.
        void stroke(int grayscale);
        void stroke(int grayscale, int alpha);
        void stroke(int r, int g, int b, int a);
        void stroke(const color& c);
        // Sets the thickness used to draw the outlines of shapes.
        void stroke_weight(float w);
        // Sets the color used to draw the inlines of shapes.
        void inner_stroke(int grayscale);
        void inner_stroke(int grayscale, int alpha);
        void inner_stroke(int r, int g, int b, int a);
        void inner_stroke(const color& c);
        // Sets the thickness used to draw the inlines of shapes.
        void inner_stroke_weight(float w);
        // Blends two colors to find a third color between them.
        color lerp_color(const color& c1, const color& c2, float t);
    }
}