#include "geometry/2d/polygon.h"
#include "geometry/geometry_helpers.h"

#include "resources/geometry_pool.h"

#include <array>
#include <sstream>

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
            self->vertex_positions()[index++] = { glm::vec3(x3, y4, 0.0f) };
        }

        //-------------------------------------------------------------------------
        geometry* extrude_polygon(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {

        }

        //-------------------------------------------------------------------------
        geometry* make_polygon(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
        {
            std::stringstream stream;

            stream << "polygon|";
            stream << x1 << "|";
            stream << y1 << "|";
            stream << x2 << "|";
            stream << y2 << "|";
            stream << x3 << "|";
            stream << y3 << "|";
            stream << x4 << "|";
            stream << y4;

            const std::string gid = stream.str();

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [x1, y1, x2, y2, x3, y3, x4, y4](geometry* self)
                {
                    compute_quad_faces(self);

                    make_vertices(self, x1, y1, x2, y2, x3, y3, x4, y4);
                    compute_quad_vertex_positions(self);
                    compute_quad_vertex_normals(self);
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