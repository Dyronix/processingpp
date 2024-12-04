#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace render
    {
        namespace brush
        {
            // Background
            void        push_background_color(const glm::vec4& color);

            glm::vec4   background();

            // Fill
            void        push_fill_color(const glm::vec4& color);
            void        push_fill_enable(bool enable);

            bool        fill_enabled();
            glm::vec4   fill();

            // Stroke
            void        push_stroke_width(f32 w);
            void        push_stroke_color(const glm::vec4& color);
            void        push_stroke_enable(bool enable);
            
            void        push_inner_stroke_width(f32 w);
            void        push_inner_stroke_color(const glm::vec4& color);
            void        push_inner_stroke_enable(bool enable);

            bool        stroke_enabled();
            f32         stroke_width();
            glm::vec4   stroke();

            bool        inner_stroke_enabled();
            f32         inner_stroke_width();
            glm::vec4   inner_stroke();

            // Tint
            void        push_tint_color(const glm::vec4& color);
            void        push_tint_enable(bool enable);

            bool        tint_enabled();
            glm::vec4   tint();
        }
    }
}