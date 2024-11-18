#include "geometry/3d/box.h"

#include "resources/geometry_pool.h"

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
        geometry* make_point()
        {
            const std::string gid = "point_3d";

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry* geom)
                {
                    make_vertices(geom);
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