#include "geometry/2d/point.h"
#include "geometry/2d/ellipse.h"

#include "resources/geometry_pool.h"

#include "util/transform_stack.h"

#include <array>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* geom)
        {
            geom->vertex_positions().assign(1, glm::vec3(0, 0, 0));
        }

        //-------------------------------------------------------------------------
        geometry* extrude_point(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {
            temp_stringstream stream;

            stream << (extrusion_width > 0 ? "point_out_stroke|" : "point_in_stroke");
            stream << extrusion_width;

            const temp_string gid = stream.str();

            geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                geom = make_ellipse();
            }
            else
            {
                geom = geometry_pool::get_geometry(gid);
            }

            return geom;
        }

        //-------------------------------------------------------------------------
        geometry* make_2d_point()
        {
            const temp_string gid = "point_2d";

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry* geom)
                {
                    make_vertices(geom);
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