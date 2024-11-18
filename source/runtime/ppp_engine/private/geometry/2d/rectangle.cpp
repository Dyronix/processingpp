#include "geometry/2d/polygon.h"
#include "geometry/geometry_helpers.h"

#include "resources/geometry_pool.h"

#include <array>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        geometry* extrude_rectangle(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {

        }

        //-------------------------------------------------------------------------
        geometry* make_rectangle()
        {
            const std::string gid = "rect";

            const geometry* geom = nullptr;

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