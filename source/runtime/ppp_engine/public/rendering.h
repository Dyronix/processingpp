#pragma once

namespace ppp
{
    void create_canvas(float x, float y, float w, float h);
    void resize_canvas(float x, float y, float w, float h);
    void no_canvas();
    void wireframe_linewidth(float line_width);
    void wireframe_color(float r, float g, float b);
    void enable_instance_draw_mode();
    void enable_batched_draw_mode();
    void enable_forward_rendering();
    void enable_deferred_rendering();
}