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
        shape(const geometry::geometry* geom, const resources::imaterial* material, bool cast_shadows)
            : m_geometry(geom)
            , m_material(material)
            , m_cast_shadows(cast_shadows)
        {}

        ~shape() override = default;

        bool has_smooth_normals() const override
        {
            return m_geometry->has_smooth_normals();
        }
        bool has_textures() const override
        {
            return m_material->textures().size() > 0;
        }
        bool cast_shadows() const override
        {
            return m_cast_shadows;
        }

        u32 vertex_count() const override
        {
            return m_geometry->vertex_count();
        }
        u32 index_count() const override
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
            return m_material->shader_tag();
        }

        const resources::imaterial* material() const override
        {
            return m_material;
        }

    private:
        const geometry::geometry*       m_geometry;
        const resources::imaterial*     m_material;

        bool                            m_cast_shadows;
    };

    //-------------------------------------------------------------------------
    shape create_shape(const geometry::geometry* geom, bool cast_shadows)
    {
        return shape(geom, material_pool::get_or_create_material_instance(render::active_shader()), cast_shadows);
    }

    //-------------------------------------------------------------------------
    shape create_shape_2d(const geometry::geometry* geom)
    {
        constexpr bool cast_shadows = false;

        return create_shape(geom, cast_shadows);
    }

    //-------------------------------------------------------------------------
    void enable_shadows()
    {
        render::enable_shadows();
    }
    //-------------------------------------------------------------------------
    void disable_shadows()
    {
        render::disable_shadows();
    }

    //-------------------------------------------------------------------------
    void ellipse_mode(shape_mode_type mode)
    {
        internal::_ellipse_mode = mode;
    }

    //-------------------------------------------------------------------------
    void rect_mode(shape_mode_type mode)
    {
        internal::_rect_mode = mode;
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
    geometry_id circle(float x, float y, float r, int detail)
    {
        return ellipse(x, y, r, r, detail);
    }

    //-------------------------------------------------------------------------
    geometry_id ellipse(float x, float y, float w, float h, int detail)
    {
        geometry::geometry* geom = geometry::make_ellipse(detail);

        shape s = create_shape_2d(geom);

        push();
        translate(x, y);

        if (internal::_ellipse_mode == shape_mode_type::CORNER)
        {
            glm::vec2 center = geometry::circular_center_translation(w, h);

            translate(-center.x, -center.y);
        }

        scale(w, h);

        render::submit_render_item(render::topology_type::TRIANGLES, &s);

        glm::mat4 world = transform_stack::active_world();

        pop();

        if (render::brush::stroke_enabled())
        {
            constexpr bool outer_stroke = true;

            auto stroke_item = geometry::extrude_ellipse(world, geom, render::brush::stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        if (render::brush::inner_stroke_enabled())
        {
            constexpr bool outer_stroke = false;

            auto stroke_item = geometry::extrude_ellipse(world, geom, -render::brush::inner_stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id line(float x1, float y1, float x2, float y2)
    {
        geometry::geometry* geom = geometry::make_line(x1, y1, x2, y2);

        shape s = create_shape_2d(geom);

        render::submit_render_item(render::topology_type::LINES, &s);

        if (render::brush::stroke_enabled())
        {
            constexpr bool outer_stroke = true;

            auto stroke_item = geometry::extrude_line(geom->vertex_positions().data(), geom->vertex_count(), render::brush::stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id point(float x, float y)
    {
        geometry::geometry* geom = geometry::make_2d_point();

        shape s = create_shape_2d(geom);

        push();
        translate(x, y);

        render::submit_render_item(render::topology_type::POINTS, &s);

        pop();

        if (render::brush::stroke_enabled())
        {
            constexpr bool outer_stroke = true;

            auto stroke_item = geometry::extrude_point(geom->vertex_positions().data(), geom->vertex_count(), render::brush::stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            push();
            translate(x, y);
            scale(render::brush::stroke_width(), render::brush::stroke_width());

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);

            pop();
        }

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id rect(float x, float y, float w, float h)
    {
        geometry::geometry* geom = geometry::make_rectangle();

        shape s = create_shape_2d(geom);

        push();
        translate(x, y);

        if (internal::_rect_mode == shape_mode_type::CORNER)
        {
            glm::vec2 center = geometry::rectanglular_center_translation(x, y, w, h);

            translate(-center.x, -center.y);
        }

        scale(w, h);

        render::submit_render_item(render::topology_type::TRIANGLES, &s);

        glm::mat4 world = transform_stack::active_world();

        pop();

        if (render::brush::stroke_enabled())
        {
            constexpr bool outer_stroke = true;

            auto stroke_item = geometry::extrude_polygon(world, geom, render::brush::stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        if (render::brush::inner_stroke_enabled())
        {
            constexpr bool outer_stroke = false;

            auto stroke_item = geometry::extrude_polygon(world, geom, -render::brush::inner_stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id square(float x, float y, float s)
    {
        return rect(x, y, s, s);
    }

    //-------------------------------------------------------------------------
    geometry_id triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        geometry::geometry* geom = geometry::make_triangle(x1, y1, x2, y2, x3, y3);

        shape s = create_shape_2d(geom);

        push();

        if (internal::_triangle_mode == shape_mode_type::CORNER)
        {
            glm::vec2 center = geometry::triangular_center_translation(x1, y1, x2, y2, x3, y3);

            translate(-center.x, -center.y);
        }

        render::submit_render_item(render::topology_type::TRIANGLES, &s);

        glm::mat4 world = transform_stack::active_world();

        pop();

        if (render::brush::stroke_enabled())
        {
            constexpr bool outer_stroke = true;

            auto stroke_item = geometry::extrude_triangle(world, geom, render::brush::stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        if (render::brush::inner_stroke_enabled())
        {
            constexpr bool outer_stroke = false;

            auto stroke_item = geometry::extrude_triangle(world, geom, -render::brush::inner_stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        geometry::geometry* geom = geometry::make_polygon(x1, y1, x2, y2, x3, y3, x4, y4);

        shape s = create_shape_2d(geom);

        push();

        if (internal::_rect_mode == shape_mode_type::CORNER)
        {
            glm::vec2 center = geometry::rectanglular_center_translation(x1, y1, x2, y2, x3, y3, x4, y4);

            translate(-center.x, -center.y);
        }

        render::submit_render_item(render::topology_type::TRIANGLES, &s);

        glm::mat4 world = transform_stack::active_world();

        pop();

        if (render::brush::stroke_enabled())
        {
            constexpr bool outer_stroke = true;

            auto stroke_item = geometry::extrude_polygon(world, geom, render::brush::stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        if (render::brush::inner_stroke_enabled())
        {
            constexpr bool outer_stroke = false;

            auto stroke_item = geometry::extrude_polygon(world, geom, -render::brush::inner_stroke_width());

            shape stroke_shape = create_shape_2d(stroke_item);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_shape, outer_stroke);
        }

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id box(float width, float height, float depth)
    {
        geometry::geometry* geom = geometry::make_box(internal::_normal_mode == normal_mode_type::SMOOTH);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(width, height, depth);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id cone(float radius, float height, int detail, bool cap)
    {
        geometry::geometry* geom = geometry::make_cone(internal::_normal_mode == normal_mode_type::SMOOTH, cap, detail);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(radius, height, radius);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id cylinder(float radius, float height, int detail, bool bottom_cap, bool top_cap)
    {
        geometry::geometry* geom = geometry::make_cylinder(internal::_normal_mode == normal_mode_type::SMOOTH, top_cap, bottom_cap, detail);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(radius, height, radius);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id plane(float width, float height)
    {
        geometry::geometry* geom = geometry::make_plane(internal::_normal_mode == normal_mode_type::SMOOTH);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(width, height, 1.0f);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id sphere(float radius, int detail)
    {
        geometry::geometry* geom = geometry::make_sphere(internal::_normal_mode == normal_mode_type::SMOOTH, detail, detail);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(radius, radius, radius);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id torus(float radius, float tube_radius, int detailx, int detaily)
    {
        geometry::geometry* geom = geometry::make_torus(internal::_normal_mode == normal_mode_type::SMOOTH, radius, tube_radius, detailx, detaily);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(radius, radius, radius);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id point(float x, float y, float z)
    {
        geometry::geometry* geom = geometry::make_3d_point();

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        translate(x, y, z);
        render::submit_render_item(render::topology_type::POINTS, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id tetrahedron(float width, float height)
    {
        geometry::geometry* geom = geometry::make_tetrahedron(internal::_normal_mode == normal_mode_type::SMOOTH);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(width, height, width);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }

    //-------------------------------------------------------------------------
    geometry_id octahedron(float width, float height)
    {
        geometry::geometry* geom = geometry::make_octahedron(internal::_normal_mode == normal_mode_type::SMOOTH);

        shape s = create_shape(geom, render::shadows_enabled());

        push();
        scale(width, height, width);
        render::submit_render_item(render::topology_type::TRIANGLES, &s);
        pop();

        return geom->id();
    }
}