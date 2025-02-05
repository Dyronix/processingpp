#include "geometry/3d/box.h"
#include "geometry/geometry_helpers.h"

#include "resources/geometry_pool.h"

#include <array>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        geometry* make_plane(bool smooth_normals)
        {
            pool_stringstream stream;

            stream << "plane|";
            stream << smooth_normals;

            const pool_string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry* geom)
                {
                    compute_quad_faces(geom);

                    compute_quad_vertex_positions(geom);
                    compute_quad_vertex_uvs(geom);
                    compute_quad_vertex_normals(geom);
                };

                return geometry_pool::add_new_geometry(geometry(gid, smooth_normals, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }
    }
}