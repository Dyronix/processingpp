#include "shapes.h"
#include "transform.h"
#include "material.h"

#include "render/render.h"
#include "render/render_types.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"

#include "util/transform_stack.h"
#include "util/brush.h"

#include "geometry/geometry.h"

#include "geometry/2d/ellipse.h"
#include "geometry/2d/line.h"
#include "geometry/2d/point.h"
#include "geometry/2d/polygon.h"
#include "geometry/2d/rectangle.h"
#include "geometry/2d/triangle.h"
#include "geometry/2d/geometry_2d_helpers.h"

#include "geometry/3d/box.h"
#include "geometry/3d/cone.h"
#include "geometry/3d/cylinder.h"
#include "geometry/3d/octahedron.h"
#include "geometry/3d/plane.h"
#include "geometry/3d/point.h"
#include "geometry/3d/sphere.h"
#include "geometry/3d/tetrahedron.h"
#include "geometry/3d/torus.h"

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

            normal_mode_type _normal_mode = normal_mode_type::FLAT;
        }

        //-------------------------------------------------------------------------
        class shape : public render::irender_item
        {
        public:
            shape(const geometry::geometry* geom, const resources::imaterial* material)
                : m_geometry(geom)
                , m_material(material)
            {}

            bool has_smooth_normals() const override
            {
                return m_geometry->has_smooth_normals();
            }
            bool has_textures() const override
            {
                return m_material->textures().size() > 0;
            }

            u64 vertex_count() const override
            {
                return m_geometry->vertex_count();
            }
            u64 index_count() const override
            {
                return m_geometry->index_count();
            }

            const std::vector<glm::vec3>& vertex_positions() const override
            {
                return m_geometry->vertex_positions();
            }
            const std::vector<glm::vec3>& vertex_normals() const override
            {
                return m_geometry->vertex_normals();
            }
            const std::vector<glm::vec2>& vertex_uvs() const override
            {
                return m_geometry->vertex_uvs();
            }

            const std::vector<render::face>& faces() const override
            {
                return m_geometry->faces();
            }

            const u64 geometry_id() const override
            {
                return m_geometry->id();
            }
            const u64 material_id() const override
            {
                return m_material->id();
            }

            const resources::imaterial* material() const override
            {
                return m_material;
            }

        private:
            const geometry::geometry* m_geometry;
            const resources::imaterial* m_material;
        };

        //-------------------------------------------------------------------------
        shape create_shape(const geometry::geometry* geom)
        {
            return shape(geom, material_pool::get_or_create_material_instance(render::active_shader()));
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
            geometry::geometry* geom = geometry::make_ellipse(detail);

            shape s = create_shape(geom);

            transform::push();
            transform::translate(x, y);
            
            if (internal::_ellipse_mode == shape_mode_type::CORNER)
            {
                glm::vec2 center = geometry::circular_center_translation(w, h);

                transform::translate(-center.x, -center.y);
            }

            transform::scale(w, h);
           
            render::submit_2d_render_item(render::topology_type::TRIANGLES, &s);

            glm::mat4 world = transform_stack::active_world();

            transform::pop();

            if (render::brush::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_ellipse(world, geom, render::brush::stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                constexpr bool outer_stroke = false;

                auto stroke_item = geometry::extrude_ellipse(world, geom, -render::brush::inner_stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
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
            geometry::geometry* geom = geometry::make_line(x1, y1, x2, y2);

            shape s = create_shape(geom);

            render::submit_2d_render_item(render::topology_type::LINES, &s);

            if (render::brush::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_line(geom->vertex_positions().data(), geom->vertex_count(), render::brush::stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void point(float x, float y)
        {
            geometry::geometry* geom = geometry::make_2d_point();

            shape s = create_shape(geom);

            transform::push();
            transform::translate(x, y);

            render::submit_2d_render_item(render::topology_type::POINTS, &s);
            
            transform::pop();

            if (render::brush::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_point(geom->vertex_positions().data(), geom->vertex_count(), render::brush::stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                transform::push();
                transform::translate(x, y);
                transform::scale(render::brush::stroke_width(), render::brush::stroke_width());

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
                
                transform::pop();
            }
        }

        //-------------------------------------------------------------------------
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
        {
            geometry::geometry* geom = geometry::make_polygon(x1, y1, x2, y2, x3, y3, x4, y4);

            shape s = create_shape(geom);

            transform::push();

            if (internal::_rect_mode == shape_mode_type::CORNER)
            {
                glm::vec2 center = geometry::rectanglular_center_translation(x1, y1, x2, y2, x3, y3, x4, y4);

                transform::translate(-center.x, -center.y);
            }

            render::submit_2d_render_item(render::topology_type::TRIANGLES, &s);

            glm::mat4 world = transform_stack::active_world();

            transform::pop();

            if (render::brush::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_polygon(world, geom, render::brush::stroke_width());

                shape stroke_shape = create_shape(stroke_item);
                
                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                constexpr bool outer_stroke = false;

                auto stroke_item = geometry::extrude_polygon(world, geom, -render::brush::inner_stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void rect(float x, float y, float w, float h)
        {
            geometry::geometry* geom = geometry::make_rectangle();

            shape s = create_shape(geom);

            transform::push();
            transform::translate(x, y);

            if (internal::_rect_mode == shape_mode_type::CORNER)
            {
                glm::vec2 center = geometry::rectanglular_center_translation(x, y, w, h);

                transform::translate(-center.x, -center.y);
            }

            transform::scale(w, h);

            render::submit_2d_render_item(render::topology_type::TRIANGLES, &s);

            glm::mat4 world = transform_stack::active_world();

            transform::pop();

            if (render::brush::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_polygon(world, geom, render::brush::stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                constexpr bool outer_stroke = false;

                auto stroke_item = geometry::extrude_polygon(world, geom, -render::brush::inner_stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }
        }
        
        //-------------------------------------------------------------------------
        void square(float x, float y, float s)
        {
            rect(x, y, s, s);
        }
        
        //-------------------------------------------------------------------------
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3)
        {
            geometry::geometry* geom = geometry::make_triangle(x1, y1, x2, y2, x3, y3);

            shape s = create_shape(geom);

            transform::push();

            if (internal::_triangle_mode == shape_mode_type::CORNER)
            {
                glm::vec2 center = geometry::triangular_center_translation(x1, y1, x2, y2, x3, y3);

                transform::translate(-center.x, -center.y);
            }

            render::submit_2d_render_item(render::topology_type::TRIANGLES, &s);

            glm::mat4 world = transform_stack::active_world();

            transform::pop();

            if (render::brush::stroke_enabled())
            {
                constexpr bool outer_stroke = true;

                auto stroke_item = geometry::extrude_triangle(world, geom, render::brush::stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                constexpr bool outer_stroke = false;

                auto stroke_item = geometry::extrude_triangle(world, geom, -render::brush::inner_stroke_width());

                shape stroke_shape = create_shape(stroke_item);

                render::submit_stroke_2d_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
            }
        }

        //-------------------------------------------------------------------------
        void point(float x, float y, float z)
        {
            geometry::geometry* geom = geometry::make_3d_point();

            shape s = create_shape(geom);

            transform::push();
            transform::translate(x, y, z);
            render::submit_3d_render_item(render::topology_type::POINTS, &s);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void box(float width, float height, float depth)
        {
            geometry::geometry* geom = geometry::make_box(internal::_normal_mode == normal_mode_type::SMOOTH);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(width, height, depth);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void cylinder(float radius, float height, int detail, bool bottom_cap, bool top_cap)
        {
            geometry::geometry* geom = geometry::make_cylinder(internal::_normal_mode == normal_mode_type::SMOOTH, top_cap, bottom_cap, detail);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(radius, height, radius);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void plane(float width, float height)
        {
            geometry::geometry* geom = geometry::make_plane(internal::_normal_mode == normal_mode_type::SMOOTH);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(width, height, 1.0f);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void sphere(float radius, int detail)
        {
            geometry::geometry* geom = geometry::make_sphere(internal::_normal_mode == normal_mode_type::SMOOTH, detail, detail);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(radius, radius, radius);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }
        
        //-------------------------------------------------------------------------
        void torus(float radius, float tube_radius, int detailx, int detaily)
        {
            geometry::geometry* geom = geometry::make_torus(internal::_normal_mode == normal_mode_type::SMOOTH, radius, tube_radius, detailx, detaily);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(radius, radius, radius);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void cone(float radius, float height, int detail, bool cap)
        {
            geometry::geometry* geom = geometry::make_cone(internal::_normal_mode == normal_mode_type::SMOOTH, cap, detail);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(radius, height, radius);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void tetrahedron(float width, float height)
        {
            geometry::geometry* geom = geometry::make_tetrahedron(internal::_normal_mode == normal_mode_type::SMOOTH);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(width, height, width);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void octahedron(float width, float height)
        {
            geometry::geometry* geom = geometry::make_octahedron(internal::_normal_mode == normal_mode_type::SMOOTH);

            shape s = create_shape(geom);

            transform::push();
            transform::scale(width, height, width);
            render::submit_3d_render_item(render::topology_type::TRIANGLES, &s);
            transform::pop();
        }

        //-------------------------------------------------------------------------
        void build_primitive_geometry(std::function<void()> callback)
        {
            const std::string& tag = render::draw_mode() == render::render_draw_mode::BATCHED
                ? shader_pool::tags::unlit_color
                : shader_pool::tags::instance_unlit_color;

            render::begin_geometry_builder(tag);

            callback();

            render::end_geometry_builder();
        }

        //-------------------------------------------------------------------------
        void build_textured_geometry(std::function<void()> callback)
        {
            const std::string& tag = render::draw_mode() == render::render_draw_mode::BATCHED
                ? shader_pool::tags::unlit_texture
                : shader_pool::tags::instance_unlit_texture;

            render::begin_geometry_builder(tag);

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