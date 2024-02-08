#include "color.h"
#include "render/render.h"
#include "util/types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace color
    {
        int red(const Color& c)
        {
            return c.red;
        }

        int green(const Color& c)
        {
            return c.green;
        }

        int blue(const Color& c)
        {
            return c.blue;
        }

        int alpha(const Color& c)
        {
            return c.alpha;
        }

        void background(int grayscale)
        {
            render::clear_color(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 1.0f);
        }

        void background(int grayscale, int alpha)
        {
            render::clear_color(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f);
        }

        void background(int r, int g, int b, int a)
        {
            render::clear_color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }

        void background(const Color& c)
        {
            render::clear_color(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f);
        }

        void clear()
        {
            render::clear(render::COLOR_BUFFER_BIT);
        }

        void clear(int grayscale) 
        {
            background(grayscale);
            render::clear(render::COLOR_BUFFER_BIT);
        }
        
        void clear(int grayscale, int alpha)
        {
            background(grayscale, alpha);
            render::clear(render::COLOR_BUFFER_BIT);
        }
        
        void clear(int r, int g, int b, int a)
        {
            background(r, g, b, a);
            render::clear(render::COLOR_BUFFER_BIT);
        }
        
        void clear(const Color& c)
        {
            background(c);
            render::clear(render::COLOR_BUFFER_BIT);
        }

        void no_fill()
        {
            render::push_fill_enable(false);
        }

        void no_stroke()
        {
            render::push_stroke_enable(false);
        }

        void fill(int grayscale)
        {
            render::push_fill_enable(true);
            render::push_fill_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 255.0f));
        }

        void fill(int grayscale, int alpha)
        {
            render::push_fill_enable(true);
            render::push_fill_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
        }

        void fill(int r, int g, int b, int a)
        {
            render::push_fill_enable(true);
            render::push_fill_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
        }

        void fill(const Color& c)
        {
            render::push_fill_enable(true);
            render::push_fill_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
        }

        void stroke(int grayscale)
        {
            render::push_stroke_enable(true);
            render::push_stroke_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 255.0f));
        }

        void stroke(int grayscale, int alpha)
        {
            render::push_stroke_enable(true);
            render::push_stroke_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
        }

        void stroke(int r, int g, int b, int a)
        {
            render::push_stroke_enable(true);
            render::push_stroke_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
        }

        void stroke(const Color& c)
        {
            render::push_stroke_enable(true);
            render::push_stroke_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
        }

        void stroke_weight(float w)
        {
            render::push_stroke_enable(true);
            render::push_stroke_width(w);
        }

        unsigned int convert_color(const glm::u8vec4& color)
        {
            unsigned int out;
            out = color.w << 24;
            out |= color.z << 16;
            out |= color.y << 8;
            out |= color.x << 0;
            return out;
        }

        unsigned int convert_color(const glm::vec4& color)
        {
            unsigned int out;
            out = (conversions::f32_to_uint8(color.x)) << 24;
            out |= (conversions::f32_to_uint8(color.y)) << 16;
            out |= (conversions::f32_to_uint8(color.z)) << 8;
            out |= (conversions::f32_to_uint8(color.w)) << 0;
            return out;
        }
    }
}