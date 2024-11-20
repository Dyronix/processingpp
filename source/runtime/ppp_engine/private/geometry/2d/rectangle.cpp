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
        geometry* extrude_rectangle(const glm::mat4& world, const geometry* in_geom, f32 extrusion_width)
        {
            std::stringstream stream;

            stream << (extrusion_width > 0 ? "rect_out_stroke|" : "rect_in_stroke|");
            stream << extrusion_width << "|";
            stream << glm::to_string(world) << "|";
            stream << in_geom;

            const std::string gid = stream.str();

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

                return geometry_pool::add_new_geometry(gid, geometry(false, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_rectangle()
        {
            const std::string gid = "rect";

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry* geom)
                {
                    compute_quad_faces(geom);

                    compute_quad_vertex_positions(geom);
                    compute_quad_vertex_uvs(geom);
                    compute_quad_vertex_normals(geom);
                };

                return geometry_pool::add_new_geometry(gid, geometry(false, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }
    }
}