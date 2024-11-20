#include "geometry/3d/box.h"

#include "resources/geometry_pool.h"

#include "constants.h"

#include <array>
#include <iomanip>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_faces(geometry* self, s32 detail_x, s32 detail_y)
        {
            //for (u32 i = 0; i < detail_x; ++i)
            //{
            //    for (u32 j = 0; j < detail_y; ++j)
            //    {
            //        u32 next_i = (i + 1) % detail_x;
            //        u32 next_j = (j + 1) % detail_y;

            //        self->faces().push_back(render::face { i * detail_y + j, next_i * detail_y + j, i * detail_y + next_j });
            //        self->faces().push_back(render::face { next_i* detail_y + j, next_i* detail_y + next_j, i* detail_y + next_j});
            //    }
            //}

            const s32 slice_count = detail_x + 1;

            u32 a, b, c, d;
            for (u32 i = 0; i < detail_y; i++)
            {
                for (u32 j = 0; j < detail_x; j++)
                {
                    a = i * slice_count + j; // + offset;
                    b = i * slice_count + j + 1; // + offset;
                    c = (i + 1) * slice_count + j + 1; // + offset;
                    d = (i + 1) * slice_count + j; // + offset;

                    self->faces().push_back(render::face{ a,b,d });
                    self->faces().push_back(render::face{ d,b,c });
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self, f32 tube_ratio, s32 detail_x, s32 detail_y)
        {
            for (s32 i = 0; i <= detail_y; i++) 
            {
                f32 v = static_cast<f32>(i) / detail_y;
                f32 phi = constants::two_pi() * v;
                f32 cos_phi = std::cos(phi);
                f32 sin_phi = std::sin(phi);
                f32 r = 1.0f + tube_ratio * cos_phi;

                for (s32 j = 0; j <= detail_x; j++) 
                {
                    f32 u = static_cast<f32>(j) / detail_x;
                    f32 theta = constants::two_pi() * u;
                    f32 cos_theta = std::cos(theta);
                    f32 sin_theta = std::sin(theta);

                    // Calculate the position (vertex) of the torus
                    glm::vec3 p(r * cos_theta, r * sin_theta, tube_ratio * sin_phi);

                    // Store the vertex in the geometry object
                    self->vertex_positions().push_back(p);
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self, s32 detail_x, s32 detail_y)
        {
            for (s32 i = 0; i <= detail_y; i++) 
            {
                f32 v = static_cast<f32>(i) / detail_y;
                for (int j = 0; j <= detail_x; j++) 
                {
                    f32 u = static_cast<f32>(j) / detail_x;
                    // Store the UV coordinates as pairs of floats
                    self->vertex_uvs().push_back({u, v});
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {
            for (size_t i = 0; i < self->vertex_count(); i++) 
            {
                const glm::vec3& p = self->vertex_positions()[i];
                float theta = std::atan2(p.y, p.x);
                float phi = std::asin(p.z / std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z));

                // Compute the normal as in the original JavaScript code
                glm::vec3 n(
                    std::cos(phi) * std::cos(theta),
                    std::cos(phi) * std::sin(theta),
                    std::sin(phi)
                );

                // Store the normal in the geometry object
                self->vertex_normals().push_back(n);
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_torus(bool smooth_normals, f32 radius, f32 tube_radius, s32 detail_x, s32 detail_y)
        {
            f32 tube_ratio = tube_radius / radius;

            std::stringstream stream;

            stream << "torus|";
            stream << smooth_normals << "|";
            stream << std::fixed << std::setprecision(4) << tube_ratio << "|";
            stream << detail_x << "|";
            stream << detail_y;

            const std::string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [tube_ratio, detail_x, detail_y](geometry* self)
                {
                    make_faces(self, detail_x, detail_y);

                    make_vertices(self, tube_ratio, detail_x, detail_y);
                    make_uvs(self, detail_x, detail_y);
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