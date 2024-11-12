#include "geometry/3d/box.h"

#include "resources/geometry_pool.h"

#include <array>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self, f32 x, f32 y, f32 z)
        {
            self->vertex_positions().assign(1, glm::vec3(x, y, z));
        }

        //-------------------------------------------------------------------------
        geometry* make_point(f32 x, f32 y, f32 z)
        {
            std::stringstream stream;

            stream << "point_3d|";
            stream << x << "|";
            stream << y << "|";
            stream << z;

            const std::string gid = stream.str();

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [x, y, z](geometry* self)
                {
                    make_vertices(self, x, y, z);
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