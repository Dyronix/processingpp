#include "color.h"
#include "mathematics.h"
#include "render/render.h"
#include "util/brush.h"
#include "util/types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace color
    {
        int red(const color& c)
        {
            return c.red;
        }

        int green(const color& c)
        {
            return c.green;
        }

        int blue(const color& c)
        {
            return c.blue;
        }

        int alpha(const color& c)
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

        void background(const color& c)
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
        
        void clear(const color& c)
        {
            background(c);
            render::clear(render::COLOR_BUFFER_BIT);
        }

        void no_fill()
        {
            render::brush::push_fill_enable(false);
        }

        void no_stroke()
        {
            render::brush::push_stroke_enable(false);
        }

        void fill(int grayscale)
        {
            render::brush::push_fill_enable(true);
            render::brush::push_fill_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 255.0f));
        }

        void fill(int grayscale, int alpha)
        {
            render::brush::push_fill_enable(true);
            render::brush::push_fill_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
        }

        void fill(int r, int g, int b, int a)
        {
            render::brush::push_fill_enable(true);
            render::brush::push_fill_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
        }

        void fill(const color& c)
        {
            render::brush::push_fill_enable(true);
            render::brush::push_fill_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
        }

        void stroke(int grayscale)
        {
            render::brush::push_stroke_enable(true);
            render::brush::push_stroke_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 255.0f));
        }

        void stroke(int grayscale, int alpha)
        {
            render::brush::push_stroke_enable(true);
            render::brush::push_stroke_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
        }

        void stroke(int r, int g, int b, int a)
        {
            render::brush::push_stroke_enable(true);
            render::brush::push_stroke_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
        }

        void stroke(const color& c)
        {
            render::brush::push_stroke_enable(true);
            render::brush::push_stroke_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
        }

        void stroke_weight(float w)
        {
            render::brush::push_stroke_enable(true);
            render::brush::push_stroke_width(w);
        }

        void inner_stroke(int grayscale)
        {
            render::brush::push_inner_stroke_enable(true);
            render::brush::push_inner_stroke_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 255.0f));
        }

        void inner_stroke(int grayscale, int alpha)
        {
            render::brush::push_inner_stroke_enable(true);
            render::brush::push_inner_stroke_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
        }

        void inner_stroke(int r, int g, int b, int a)
        {
            render::brush::push_inner_stroke_enable(true);
            render::brush::push_inner_stroke_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
        }

        void inner_stroke(const color& c)
        {
            render::brush::push_inner_stroke_enable(true);
            render::brush::push_inner_stroke_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
        }

        void inner_stroke_weight(float w)
        {
            render::brush::push_inner_stroke_enable(true);
            render::brush::push_inner_stroke_width(w);
        }

        color lerp_color(const color& c1, const color& c2, float t)
        {
            // Clamp amt between 0 and 1 to prevent extrapolation
            t = std::clamp(t, 0.0f, 1.0f);

            color result;

            result.red = static_cast<int>(math::lerp((float)c1.red, (float)c2.red, t));
            result.green = static_cast<int>(math::lerp((float)c1.green, (float)c2.green, t));
            result.blue = static_cast<int>(math::lerp((float)c1.blue, (float)c2.blue, t));
            result.alpha = static_cast<int>(math::lerp((float)c1.alpha, (float)c2.alpha, t));

            return result;
        }
    }
}