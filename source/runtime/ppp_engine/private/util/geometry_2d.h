#pragma once

#include "render/render.h"

#include "util/types.h"

namespace ppp
{
    namespace geometry
    {
        namespace image
        {
            render::RenderItem extrude_image(render::VertexPosTex* vertices, s32 vertex_count, f32 extrusion_width);
            
            render::ImageItem make_image(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 image_id);
        }

        namespace font
        {
            render::ImageItem make_font(bool from_corner, f32 x, f32 y, f32 w, f32 h, f32 uv_start_x, f32 uv_start_y, f32 uv_end_x, f32 uv_end_y, s32 image_id);
        }

        render::RenderItem extrude_point(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width);
        render::RenderItem extrude_line(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width);
        render::RenderItem extrude_ellipse(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width);
        render::RenderItem extrude_polygon(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width);
        render::RenderItem extrude_triangle(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width);

        render::RenderItem make_point(f32 x, f32 y);
        render::RenderItem make_line(f32 x1, f32 y1, f32 x2, f32 y2);
        render::RenderItem make_ellipse(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 detail = 25);
        render::RenderItem make_polygon(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4);
        render::RenderItem make_triangle(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3);
    }
}