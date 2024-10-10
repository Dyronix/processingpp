#pragma once

#include "render/render.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        namespace image
        {
            render::render_item extrude_image(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);
            
            render::render_item make_image(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 image_id);
        }

        namespace font
        {
            render::render_item make_font(bool from_corner, f32 x, f32 y, f32 w, f32 h, f32 uv_start_x, f32 uv_start_y, f32 uv_end_x, f32 uv_end_y, s32 image_id);
        }

        render::render_item extrude_point(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);
        render::render_item extrude_line(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);
        render::render_item extrude_ellipse(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);
        render::render_item extrude_polygon(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);
        render::render_item extrude_triangle(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);

        render::render_item make_point(f32 x, f32 y);
        render::render_item make_line(f32 x1, f32 y1, f32 x2, f32 y2);
        render::render_item make_ellipse(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 detail = 25);
        render::render_item make_polygon(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4);
        render::render_item make_triangle(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3);
    }
}