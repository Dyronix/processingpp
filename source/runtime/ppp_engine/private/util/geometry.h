#pragma once

#include "render/render.h"

namespace ppp
{
    namespace geometry
    {
        namespace image
        {
            render::RenderItem extrude_image(render::VertexPosTex* vertices, int vertex_count, float extrusion_width);
            render::ImageItem make_image(bool from_corner, float x, float y, float w, float h, s32 image_id);
        }

        render::RenderItem extrude_point(render::VertexPos* vertices, int vertex_count, float extrusion_width);
        render::RenderItem extrude_line(render::VertexPos* vertices, int vertex_count, float extrusion_width);
        render::RenderItem extrude_ellipse(render::VertexPos* vertices, int vertex_count, float extrusion_width);
        render::RenderItem extrude_polygon(render::VertexPos* vertices, int vertex_count, float extrusion_width);
        render::RenderItem extrude_triangle(render::VertexPos* vertices, int vertex_count, float extrusion_width);

        render::RenderItem make_point(float x, float y);
        render::RenderItem make_line(float x1, float y1, float x2, float y2);
        render::RenderItem make_ellipse(bool from_corner, float x, float y, float w, float h, int detail = 25);
        render::RenderItem make_polygon(bool from_corner, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        render::RenderItem make_triangle(bool from_corner, float x1, float y1, float x2, float y2, float x3, float y3);
    }
}