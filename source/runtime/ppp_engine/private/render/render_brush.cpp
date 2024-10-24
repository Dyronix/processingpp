#include "render/render_brush.h"

#include "util/color_ops.h"

namespace ppp
{
    namespace render
    {
        namespace brush
        {
            namespace internal
            {
                //-------------------------------------------------------------------------
                s32     _fill_color             = 0xFFFFFFFF;
                bool    _fill_enable            = true;

                f32     _stroke_width           = 1.0f;
                s32     _stroke_color           = 0xFF000000;
                bool    _stroke_enable          = false;

                f32     _inner_stroke_width     = 1.0f;
                s32     _inner_stroke_color     = 0xFF000000;
                bool    _inner_stroke_enable    = false;

                s32     _tint_color             = 0xFFFFFFFF;
                bool    _tint_enable            = false;

                s32     _bg_color               = 0xFF000000;
            }

            // Background
            //-------------------------------------------------------------------------
            void push_background_color(const glm::vec4& color)
            {
                internal::_bg_color = color::convert_color(color);
            }

            glm::vec4 background()
            {
                return color::convert_color(internal::_bg_color);
            }

            // Fill
            //-------------------------------------------------------------------------
            void push_fill_color(const glm::vec4& color)
            {
                internal::_fill_color = color::convert_color(color);
            }

            //-------------------------------------------------------------------------
            void push_fill_enable(bool enable)
            {
                internal::_fill_enable = enable;
            }

            //-------------------------------------------------------------------------
            bool fill_enabled()
            {
                return internal::_fill_enable;
            }

            //-------------------------------------------------------------------------
            glm::vec4 fill()
            {
                return color::convert_color(internal::_fill_enable ? internal::_fill_color : internal::_bg_color);
            }

            // Stroke
            //-------------------------------------------------------------------------
            void push_stroke_width(f32 w)
            {
                internal::_stroke_width = w;
            }

            //-------------------------------------------------------------------------
            void push_stroke_color(const glm::vec4& color)
            {
                internal::_stroke_color = color::convert_color(color);
            }

            //-------------------------------------------------------------------------
            void push_stroke_enable(bool enable)
            {
                internal::_stroke_enable = enable;
            }

            //-------------------------------------------------------------------------
            void push_inner_stroke_width(f32 w)
            {
                internal::_inner_stroke_width = w;
            }

            //-------------------------------------------------------------------------
            void push_inner_stroke_color(const glm::vec4& color)
            {
                internal::_inner_stroke_color = color::convert_color(color);
            }

            //-------------------------------------------------------------------------
            void push_inner_stroke_enable(bool enable)
            {
                internal::_inner_stroke_enable = enable;
            }

            //-------------------------------------------------------------------------
            bool stroke_enabled()
            {
                return internal::_stroke_enable;
            }

            //-------------------------------------------------------------------------
            f32 stroke_width()
            {
                return internal::_stroke_width;
            }

            //-------------------------------------------------------------------------
            glm::vec4 stroke()
            {
                return color::convert_color(internal::_stroke_color);
            }

            //-------------------------------------------------------------------------
            bool inner_stroke_enabled()
            {
                return internal::_inner_stroke_enable;
            }

            //-------------------------------------------------------------------------
            f32 inner_stroke_width()
            {
                return internal::_inner_stroke_width;
            }

            //-------------------------------------------------------------------------
            glm::vec4 inner_stroke()
            {
                return color::convert_color(internal::_inner_stroke_color);
            }

            // Tint
            //-------------------------------------------------------------------------
            void push_tint_color(const glm::vec4& color)
            {
                internal::_tint_color = color::convert_color(color);
            }

            //-------------------------------------------------------------------------
            void push_tint_enable(bool enable)
            {
                internal::_tint_enable = enable;
            }

            //-------------------------------------------------------------------------
            bool tint_enabled()
            {
                return internal::_tint_enable;
            }

            //-------------------------------------------------------------------------
            glm::vec4 tint()
            {
                return color::convert_color(internal::_tint_color);
            }
        }
    }
}