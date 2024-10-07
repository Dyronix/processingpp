#include "shapes.h"
#include "render/render.h"
#include "util/geometry_2d.h"
#include "util/geometry_3d.h"

#include <glm/glm.hpp>

#include <array>

namespace ppp
{
    namespace shapes
    {
        namespace internal
        {
            ShapeMode _rect_mode = ShapeMode::CORNER;
            ShapeMode _ellipse_mode = ShapeMode::CENTER;
            ShapeMode _triangle_mode = ShapeMode::CENTER;
        }

        //-------------------------------------------------------------------------
        void rect_mode(ShapeMode mode)
        {
            internal::_rect_mode = mode;
        }

        //-------------------------------------------------------------------------
        void ellipse_mode(ShapeMode mode)
        {
            internal::_ellipse_mode = mode;
        }

        //-------------------------------------------------------------------------
        void triangle_mode(ShapeMode mode)
        {
            internal::_triangle_mode = mode;
        }

        //-------------------------------------------------------------------------
        void rasterization_mode(RasterizationMode mode)
        {
            if ((s32)mode & (s32)RasterizationMode::SOLID)
            {
                render::push_solid_rendering(true);
            }
            else
            {
                render::push_solid_rendering(false);
            }

            if ((s32)mode & (s32)RasterizationMode::WIREFRAME)
            {
                render::push_wireframe_rendering(true);
            }
            else
            {
                render::push_wireframe_rendering(false);
            }
        }

        //-------------------------------------------------------------------------
        void ellipse(float x, float y, float w, float h, int detail)
        {
            render::RenderItem item = geometry::make_ellipse(internal::_ellipse_mode == ShapeMode::CORNER, x, y, w, h, detail);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);

            if (render::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_ellipse(item.vertices, item.vertex_count, render::stroke_width());

                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }

            if (render::inner_stroke_enabled())
            {
                constexpr bool outer_stroke = false;

                auto stroke_item = geometry::extrude_ellipse(item.vertices, item.vertex_count, -render::inner_stroke_width());

                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void circle(float x, float y, float r, int detail)
        {
            ellipse(x, y, r, r, detail);
        }
        
        //-------------------------------------------------------------------------
        void line(float x1, float y1, float x2, float y2)
        {
            render::RenderItem item = geometry::make_line(x1, y1, x2, y2);

            render::submit_render_item(render::TopologyType::LINES, item);

            if (render::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                render::RenderItem stroke_item = geometry::extrude_line(item.vertices, item.vertex_count, render::stroke_width());

                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void point(float x, float y)
        {
            render::RenderItem item = geometry::make_point(x, y);

            render::submit_render_item(render::TopologyType::POINTS, item);

            if (render::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                render::RenderItem stroke_item = geometry::extrude_point(item.vertices, item.vertex_count, render::stroke_width());

                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
        {
            render::RenderItem item = geometry::make_polygon(internal::_rect_mode == ShapeMode::CORNER, x1, y1, x2, y2, x3, y3, x4, y4);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);

            if (render::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_polygon(item.vertices, item.vertex_count, render::stroke_width());
                
                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }

            if (render::inner_stroke_enabled())
            {
                constexpr bool outer_stroke = false;

                render::RenderItem stroke_item = geometry::extrude_polygon(item.vertices, item.vertex_count, -render::inner_stroke_width());

                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void rect(float x, float y, float w, float h)
        {
            polygon(x, y, x + w, y, x + w, y + h, x, y + h);
        }
        
        //-------------------------------------------------------------------------
        void square(float x, float y, float s)
        {
            rect(x, y, s, s);
        }
        
        //-------------------------------------------------------------------------
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3)
        {
            render::RenderItem item = geometry::make_triangle(internal::_triangle_mode == ShapeMode::CORNER, x1, y1, x2, y2, x3, y3);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);

            if (render::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_triangle(item.vertices, item.vertex_count, render::stroke_width());

                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }

            if (render::inner_stroke_enabled())
            {
                constexpr bool outer_stroke = false;

                render::RenderItem stroke_item = geometry::extrude_triangle(item.vertices, item.vertex_count, -render::inner_stroke_width());

                render::submit_stroke_render_item(render::TopologyType::TRIANGLES, stroke_item, outer_stroke);
            }
        }

        //-------------------------------------------------------------------------
        void box(float width, float height, float depth)
        {
            render::RenderItem item = geometry::make_box(width, height, depth);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void cylinder(float radius, float height, float detail, bool bottom_cap, bool top_cap)
        {
            render::RenderItem item = geometry::make_cylinder(radius, height, detail, bottom_cap, top_cap);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void plane(float width, float height)
        {
            render::RenderItem item = geometry::make_plane(width, height);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void sphere(float radius, float detail)
        {
            render::RenderItem item = geometry::make_sphere(radius, detail);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void torus(float radius, float tube_radius, float detailx, float detaily)
        {
            render::RenderItem item = geometry::make_torus(radius, tube_radius, detailx, detaily);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }

        //-------------------------------------------------------------------------
        void cone(float radius, float height, float detail, bool cap)
        {
            render::RenderItem item = geometry::make_cone(radius, height, detail, cap);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }

        //-------------------------------------------------------------------------
        void tetrahedron(float width, float height)
        {
            render::RenderItem item = geometry::make_tetrahedron(width, height);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }

        //-------------------------------------------------------------------------
        void octahedron(float width, float height)
        {
            render::RenderItem item = geometry::make_octahedron(width, height);

            render::submit_render_item(render::TopologyType::TRIANGLES, item);
        }
    }
}