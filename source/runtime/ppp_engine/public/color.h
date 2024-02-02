#pragma once

#include <array>

namespace ppp
{
    namespace color
    {
        struct Color
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

        int red(const Color& c);
        int green(const Color& c);
        int blue(const Color& c);
        int alpha(const Color& c);

        void background(int grayscale);
        void background(int grayscale, int alpha);
        void background(int r, int g, int b, int a);
        void background(const Color& c);

        void clear();
        void clear(int grayscale);
        void clear(int grayscale, int alpha);
        void clear(int r, int g, int b, int a);
        void clear(const Color& c);

        void no_fill();
        void no_stroke();

        void fill(int grayscale);
        void fill(int grayscale, int alpha);
        void fill(int r, int g, int b, int a);
        void fill(const Color& c);
        void stroke(int grayscale);
        void stroke(int grayscale, int alpha);
        void stroke(int r, int g, int b, int a);
        void stroke(const Color& c);
    }
}