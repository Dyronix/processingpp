#include "geometry/2d/triangle.h"

#include "resources/geometry_pool.h"

#include <array>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_faces(geometry* geom)
        {
            geom->faces().assign(1, { 0, 1, 2 });
        }

        //-------------------------------------------------------------------------
        static void make_vertices(geometry* geom, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
        {
            s32 index = 0;

            geom->vertex_positions()[index++] = glm::vec3(x1, y1, 0);
            geom->vertex_positions()[index++] = glm::vec3(x2, y2, 0);
            geom->vertex_positions()[index++] = glm::vec3(x3, y3, 0);
        }

        //-------------------------------------------------------------------------
        static void make_uvs(geometry* geom)
        {
            s32 index = 0;

            geom->vertex_uvs()[index++] = glm::vec2(0.0f, 0.0f);
            geom->vertex_uvs()[index++] = glm::vec2(1.0f, 0.0f);
            geom->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
        }

        //-------------------------------------------------------------------------
        static void make_normals(geometry* geom)
        {
            geom->compute_normals();
        }

        //-------------------------------------------------------------------------
        geometry* extrude_triangle(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {

        }

        //-------------------------------------------------------------------------
        geometry* make_triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
        {
            std::stringstream stream;

            stream << "triangle|";
            stream << x1 << "|";
            stream << y1 << "|";
            stream << x2 << "|";
            stream << y2 << "|";
            stream << x3 << "|";
            stream << y3;

            const std::string gid = stream.str();

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [x1, y1, x2, y2, x3, y3](geometry* geom)
                {
                    make_faces(geom);

                    make_vertices(geom, x1, y1, x2, y2, x3, y3);
                    make_uvs(geom);
                    make_normals(geom);
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