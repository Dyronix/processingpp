#include "shapes.h"
#include "render/render.h"
#include "render/render_brush.h"
#include "render/render_types.h"
#include "resources/shader_pool.h"
#include "geometry/2d/geometry_2d.h"

#include "geometry/3d/box.h"
#include "geometry/3d/cone.h"
#include "geometry/3d/cylinder.h"
#include "geometry/3d/octahedron.h"
#include "geometry/3d/plane.h"
#include "geometry/3d/point.h"
#include "geometry/3d/sphere.h"
#include "geometry/3d/tetrahedron.h"
#include "geometry/3d/torus.h"

#include "transform.h"

#include <glm/glm.hpp>

#include <array>

namespace ppp
{
    namespace shapes
    {
        namespace internal
        {
            shape_mode_type _rect_mode = shape_mode_type::CORNER;
            shape_mode_type _ellipse_mode = shape_mode_type::CENTER;
            shape_mode_type _triangle_mode = shape_mode_type::CENTER;

            normal_mode_type _normal_mode = normal_mode_type::SMOOTH;
        }

        //-------------------------------------------------------------------------
        void rect_mode(shape_mode_type mode)
        {
            internal::_rect_mode = mode;
        }

        //-------------------------------------------------------------------------
        void ellipse_mode(shape_mode_type mode)
        {
            internal::_ellipse_mode = mode;
        }

        //-------------------------------------------------------------------------
        void triangle_mode(shape_mode_type mode)
        {
            internal::_triangle_mode = mode;
        }

        //-------------------------------------------------------------------------
        void normal_mode(normal_mode_type mode)
        {
            internal::_normal_mode = mode;
        }

        //-------------------------------------------------------------------------
        void enable_wireframe_mode(bool enable)
        {
            render::push_wireframe_rendering(enable);
        }

        //-------------------------------------------------------------------------
        void enable_solid_mode(bool enable)
        {
            render::push_solid_rendering(enable);
        }

        //-------------------------------------------------------------------------
        void ellipse(float x, float y, float w, float h, int detail)
        {
            render::render_item item = geometry::make_ellipse(internal::_ellipse_mode == shape_mode_type::CORNER, x, y, w, h, detail);

            render::submit_render_item(render::topology_type::TRIANGLES, item);

            if (render::brush::stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = true;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                auto stroke_item = geometry::extrude_ellipse(vertex_positions, vert_comp->vertex_count(), render::brush::stroke_width());

                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = false;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                auto stroke_item = geometry::extrude_ellipse(vertex_positions, vert_comp->vertex_count(), -render::brush::inner_stroke_width());

                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
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
            render::render_item item = geometry::make_line(x1, y1, x2, y2);

            render::submit_render_item(render::topology_type::LINES, item);

            if (render::brush::stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = true;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                render::render_item stroke_item = geometry::extrude_line(vertex_positions, vert_comp->vertex_count(), render::brush::stroke_width());

                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void point(float x, float y)
        {
            render::render_item item = geometry::make_point(x, y);

            render::submit_render_item(render::topology_type::POINTS, item);

            if (render::brush::stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = true;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                render::render_item stroke_item = geometry::extrude_point(vertex_positions, vert_comp->vertex_count(), render::brush::stroke_width());

                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
            }
        }

        //-------------------------------------------------------------------------
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
        {
            render::render_item item = geometry::make_polygon(internal::_rect_mode == shape_mode_type::CORNER, x1, y1, x2, y2, x3, y3, x4, y4);

            render::submit_render_item(render::topology_type::TRIANGLES, item);

            if (render::brush::stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = true;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                auto stroke_item = geometry::extrude_polygon(vertex_positions, vert_comp->vertex_count(), render::brush::stroke_width());
                
                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = false;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                render::render_item stroke_item = geometry::extrude_polygon(vertex_positions, vert_comp->vertex_count(), -render::brush::inner_stroke_width());

                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
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
            render::render_item item = geometry::make_triangle(internal::_triangle_mode == shape_mode_type::CORNER, x1, y1, x2, y2, x3, y3);

            render::submit_render_item(render::topology_type::TRIANGLES, item);

            if (render::brush::stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = true;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                auto stroke_item = geometry::extrude_triangle(vertex_positions, vert_comp->vertex_count(), render::brush::stroke_width());

                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = false;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                render::render_item stroke_item = geometry::extrude_triangle(vertex_positions, vert_comp->vertex_count(), -render::brush::inner_stroke_width());

                render::submit_stroke_render_item(render::topology_type::TRIANGLES, stroke_item, outer_stroke);
            }
        }

        //-------------------------------------------------------------------------
        void point(float x, float y, float z)
        {
            geometry::geometry* geom = geometry::make_point();

            transform::push();
            transform::translate(x, y, z);
            render::submit_render_item(render::topology_type::POINTS, geom);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void box(float width, float height, float depth)
        {
            geometry::geometry* geom = geometry::make_box(internal::_normal_mode == normal_mode_type::SMOOTH);

            transform::push();
            transform::scale(width, height, depth);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void cylinder(float radius, float height, int detail, bool bottom_cap, bool top_cap)
        {
            geometry::geometry* geom = geometry::make_cylinder(internal::_normal_mode == normal_mode_type::SMOOTH, top_cap, bottom_cap, detail);

            transform::push();
            transform::scale(radius, height, radius);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void plane(float width, float height)
        {
            geometry::geometry* geom = geometry::make_plane(internal::_normal_mode == normal_mode_type::SMOOTH);

            transform::push();
            transform::scale(width, height, 1.0f);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void sphere(float radius, int detail)
        {
            geometry::geometry* geom = geometry::make_sphere(internal::_normal_mode == normal_mode_type::SMOOTH, detail, detail);

            transform::push();
            transform::scale(radius, radius, radius);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void torus(float radius, float tube_radius, int detailx, int detaily)
        {
            geometry::geometry* geom = geometry::make_torus(internal::_normal_mode == normal_mode_type::SMOOTH, radius, tube_radius, detailx, detaily);

            transform::push();
            transform::scale(radius, radius, radius);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void cone(float radius, float height, int detail, bool cap)
        {
            geometry::geometry* geom = geometry::make_cone(internal::_normal_mode == normal_mode_type::SMOOTH, cap, detail);

            transform::push();
            transform::scale(radius, height, radius);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void tetrahedron(float width, float height)
        {
            geometry::geometry* geom = geometry::make_tetrahedron(internal::_normal_mode == normal_mode_type::SMOOTH);

            transform::push();
            transform::scale(width, height, width);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void octahedron(float width, float height)
        {
            geometry::geometry* geom = geometry::make_octahedron(internal::_normal_mode == normal_mode_type::SMOOTH);

            transform::push();
            transform::scale(width, height, width);
            render::submit_render_item(render::topology_type::TRIANGLES, geom);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void build_primitive_geometry(std::function<void()> callback)
        {
            render::begin_geometry_builder(shader_pool::tags::unlit_color);

            callback();

            render::end_geometry_builder();
        }

        //-------------------------------------------------------------------------
        void build_textured_geometry(std::function<void()> callback)
        {
            render::begin_geometry_builder(shader_pool::tags::unlit_texture);

            callback();

            render::end_geometry_builder();
        }

        //-------------------------------------------------------------------------
        void build_custom_geometry(const std::string& tag, std::function<void()> callback)
        {
            render::begin_geometry_builder(tag);

            callback();

            render::end_geometry_builder();
        }
    }
}