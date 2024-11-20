#include "geometry/2d/line.h"
#include "geometry/2d/polygon.h"

#include "resources/geometry_pool.h"

#include <array>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* geom, f32 x1, f32 y1, f32 x2, f32 y2)
        {
            geom->vertex_positions().assign(2, glm::vec3(0, 0, 0));

            s32 index = 0;

            geom->vertex_positions()[index++] = glm::vec3(x1, y1, 0.0f);
            geom->vertex_positions()[index++] = glm::vec3(x2, y2, 0.0f);
        }

        //-------------------------------------------------------------------------
        geometry* extrude_line(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {
            const std::string gid = (extrusion_width > 0 ? "line_out_stroke|" : "line_in_stroke") + std::to_string(extrusion_width);

            if (!geometry_pool::has_geometry(gid))
            {
                const f32 stroke_x1 = vertices[0].x - extrusion_width;
                const f32 stroke_y1 = vertices[0].y - extrusion_width;

                const f32 stroke_x2 = vertices[1].x + extrusion_width;
                const f32 stroke_y2 = vertices[1].y - extrusion_width;

                const f32 stroke_x3 = vertices[1].x + extrusion_width;
                const f32 stroke_y3 = vertices[1].y + extrusion_width;

                const f32 stroke_x4 = vertices[0].x - extrusion_width;
                const f32 stroke_y4 = vertices[0].y + extrusion_width;

                return make_polygon(stroke_x1, stroke_y1, stroke_x2, stroke_y2, stroke_x3, stroke_y3, stroke_x4, stroke_y4);
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_line(f32 x1, f32 y1, f32 x2, f32 y2)
        {
            std::stringstream stream;

            stream << "line|";
            stream << x1 << "|";
            stream << y1 << "|";
            stream << x2 << "|";
            stream << y2;

            const std::string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [x1, y1, x2, y2](geometry* geom)
                {
                    make_vertices(geom, x1, y1, x2, y2);
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