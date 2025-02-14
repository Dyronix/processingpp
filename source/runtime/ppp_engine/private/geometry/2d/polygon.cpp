#include "geometry/2d/polygon.h"
#include "geometry/2d/geometry_2d_helpers.h"
#include "geometry/geometry_helpers.h"

#include "resources/geometry_pool.h"

#include <array>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
        {
            self->vertex_positions().assign(4, glm::vec3());

            u64 index = 0;

            self->vertex_positions()[index++] = { glm::vec3(x1, y1, 0.0f) };
            self->vertex_positions()[index++] = { glm::vec3(x2, y2, 0.0f) };
            self->vertex_positions()[index++] = { glm::vec3(x3, y3, 0.0f) };
            self->vertex_positions()[index++] = { glm::vec3(x4, y4, 0.0f) };
        }

        //-------------------------------------------------------------------------
        geometry* extrude_polygon(const glm::mat4& world, const geometry* in_geom, f32 extrusion_width)
        {
            temp_stringstream stream;

            stream << (extrusion_width > 0 ? "polygon_out_stroke|" : "polygon_in_stroke|");
            stream << extrusion_width << "|";
            stream << glm::to_string(world) << "|";
            stream << in_geom;

            const temp_string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [&world, in_geom, extrusion_width](geometry* geom)
                {
                    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

                    for (s32 i = 0; i < in_geom->vertex_count(); ++i)
                    {
                        center += in_geom->vertex_positions()[i];
                    }

                    center /= in_geom->vertex_count();

                    extrude_vertices(geom->vertex_positions(), world, center, in_geom->vertex_positions().data(), in_geom->vertex_count(), extrusion_width);
                    extrude_indices(geom->faces(), in_geom->vertex_count());
                };

                return geometry_pool::add_new_geometry(geometry(gid, false, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_polygon(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
        {
            temp_stringstream stream;

            stream << "polygon|";
            stream << x1 << "|";
            stream << y1 << "|";
            stream << x2 << "|";
            stream << y2 << "|";
            stream << x3 << "|";
            stream << y3 << "|";
            stream << x4 << "|";
            stream << y4;

            const temp_string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [x1, y1, x2, y2, x3, y3, x4, y4](geometry* self)
                {
                    compute_quad_faces(self);

                    make_vertices(self, x1, y1, x2, y2, x3, y3, x4, y4);
                    compute_quad_vertex_uvs(self);
                    compute_quad_vertex_normals(self);
                };

                return geometry_pool::add_new_geometry(geometry(gid, false, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }
    }
}