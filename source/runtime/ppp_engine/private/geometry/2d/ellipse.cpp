#include "geometry/2d/ellipse.h"
#include "geometry/2d/geometry_2d_helpers.h"

#include "resources/geometry_pool.h"

#include "constants.h"

#include <array>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_faces(geometry* geom, s32 detail)
        {
            s32 total_nr_faces = detail;

            geom->faces().assign(total_nr_faces, render::face());

            s32 i = 0;
            for (s32 t = 0; t < total_nr_faces; ++t)
            {
                geom->faces()[t][0] = 0;
                geom->faces()[t][1] = i + 1;
                geom->faces()[t][2] = i + 2;

                ++i;
            }

            // Loop back to the first triangle
            geom->faces()[total_nr_faces - 1][2] = 1;
        }

        //-------------------------------------------------------------------------
        static void make_vertices(geometry* geom, s32 detail)
        {
            f32 r = 1.0f;
            s32 total_nr_vertices = detail + 1; // take center point into account

            geom->vertex_positions().assign(total_nr_vertices, glm::vec3(0.0f, 0.0f, 0.0f));
            geom->vertex_positions()[0] = glm::vec3{ 0.0f, 0.0f, 0.0f };

            for (s32 t = 1; t < total_nr_vertices; ++t)
            {
                f32 angle = (t / static_cast<f32>(detail)) * constants::two_pi();

                f32 v_x = cos(angle) * r;
                f32 v_y = sin(angle) * r;

                geom->vertex_positions()[t] = glm::vec3(v_x, v_y, 0.0f);
            }
        }

        //-------------------------------------------------------------------------
        static void make_uvs(geometry* geom, s32 detail)
        {
            f32 start = 0.0f;
            f32 stop = constants::two_pi();

            s32 total_nr_vertices = detail + 1; // take center point into account

            geom->vertex_uvs().assign(total_nr_vertices, glm::vec2(0.0f, 0.0f));
            geom->vertex_uvs()[0] = glm::vec2 { 0.5f, 0.5f };

            // vertices for the perimeter of the circle
            for (s32 i = 1; i < total_nr_vertices; i++)
            {
                const f32 u = i / (f32)detail;
                const f32 theta = (stop - start) * u + start;

                const f32 x = 0.5 + std::cos(theta) / 2;
                const f32 y = 0.5 + std::sin(theta) / 2;

                geom->vertex_uvs()[i] = glm::vec2(x, y);
            }
        }

        //-------------------------------------------------------------------------
        static void make_normals(geometry* geom)
        {
            geom->compute_normals();
        }

        //-------------------O------------------------------------------------------
        geometry* extrude_ellipse(const glm::mat4& world, const geometry* in_geom, f32 extrusion_width)
        {
            std::stringstream stream;

            stream << (extrusion_width > 0 ? "elli_out_stroke|" : "elli_in_stroke|");
            stream << extrusion_width << "|";
            stream << glm::to_string(world) << "|";
            stream << in_geom;

            const std::string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [&world, in_geom, extrusion_width](geometry* geom)
                {
                    const glm::vec3& center = in_geom->vertex_positions()[0];

                    extrude_vertices(geom->vertex_positions(), world, center, &in_geom->vertex_positions()[1], in_geom->vertex_count() - 1, extrusion_width);
                    extrude_indices(geom->faces(), in_geom->vertex_count() - 1);
                };

                return geometry_pool::add_new_geometry(geometry(gid, false, create_geom_fn));
            }
            else
            {
                return geometry_pool::get_geometry(gid);
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_ellipse(s32 detail)
        {
            std::stringstream stream;

            stream << "ellipse|";
            stream << detail;

            const std::string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [detail](geometry* geom)
                {
                    make_faces(geom, detail);

                    make_vertices(geom, detail);
                    make_uvs(geom, detail);
                    make_normals(geom);
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