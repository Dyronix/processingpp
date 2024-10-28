#include "shapes.h"
#include "render/render.h"
#include "render/render_brush.h"
#include "render/render_types.h"
#include "resources/shader_pool.h"
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
        void point(float x, float y, float z)
        {
            render::render_item item = geometry::make_point(x, y, z);

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
        void box(float width, float height, float depth)
        {
            render::render_item& item = geometry::make_box(width, height, depth, internal::_normal_mode == normal_mode_type::SMOOTH);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void cylinder(float radius, float height, float detail, bool bottom_cap, bool top_cap)
        {
            render::render_item item = geometry::make_cylinder(radius, height, internal::_normal_mode == normal_mode_type::SMOOTH, detail, bottom_cap, top_cap);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void plane(float width, float height)
        {
            render::render_item item = geometry::make_plane(width, height);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void sphere(float radius, float detail)
        {
            render::render_item item = geometry::make_sphere(radius, internal::_normal_mode == normal_mode_type::SMOOTH, detail);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
        }
        
        //-------------------------------------------------------------------------
        void torus(float radius, float tube_radius, float detailx, float detaily)
        {
            render::render_item item = geometry::make_torus(radius, tube_radius, internal::_normal_mode == normal_mode_type::SMOOTH, detailx, detaily);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
        }

        //-------------------------------------------------------------------------
        void cone(float radius, float height, float detail, bool cap)
        {
            render::render_item item = geometry::make_cone(radius, height, internal::_normal_mode == normal_mode_type::SMOOTH, detail, cap);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
        }

        //-------------------------------------------------------------------------
        void tetrahedron(float width, float height)
        {
            render::render_item item = geometry::make_tetrahedron(width, height, internal::_normal_mode == normal_mode_type::SMOOTH);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
        }

        //-------------------------------------------------------------------------
        void octahedron(float width, float height)
        {
            render::render_item item = geometry::make_octahedron(width, height, internal::_normal_mode == normal_mode_type::SMOOTH);

            render::submit_render_item(render::topology_type::TRIANGLES, item);
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