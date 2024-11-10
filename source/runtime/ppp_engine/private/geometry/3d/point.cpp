#include "geometry/3d/box.h"

#include "resources/geometry_pool.h"

#include <array>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_faces(geometry* self)
        {

        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self)
        {

        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self)
        {

        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {

        }

        //-------------------------------------------------------------------------
        geometry* make_point(bool smooth_normals)
        {
            const std::string gid = "point|" + std::to_string(smooth_normals);

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry* self)
                {
                    make_faces(self);

                    make_vertices(self);
                    make_uvs(self);
                    make_normals(self);
                };

                return geometry_pool::add_new_geometry(gid, geometry(smooth_normals, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }
    }
}