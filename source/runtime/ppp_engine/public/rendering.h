#pragma once

namespace ppp
{
    namespace rendering
    {
        void create_canvas(float x, float y, float w, float h);
        void resize_canvas(float x, float y, float w, float h);
        void no_canvas();
        void wireframe_linewidth(float line_width);
        void wireframe_color(float r, float g, float b);
    }
}