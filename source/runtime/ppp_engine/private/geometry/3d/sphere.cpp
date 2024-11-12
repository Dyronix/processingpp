#include "geometry/3d/box.h"

#include "resources/geometry_pool.h"

#include "constants.h"

#include <array>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_faces(geometry* self, s32 detail_x, s32 detail_y)
        {
            for (s32 i = 0; i < detail_y; ++i) 
            {
                for (s32 j = 0; j < detail_x; ++j) 
                {
                    // Calculate the four corners of each quad
                    u32 top_left = i * (detail_x + 1) + j;
                    u32 top_right = top_left + 1;
                    u32 bottom_left = (i + 1) * (detail_x + 1) + j;
                    u32 bottom_right = bottom_left + 1;

                    // First Triangle: (bottom_left, top_left, top_right)
                    // Second Triangle: (bottom_right, bottom_left, top_right)
                    self->faces().push_back({ bottom_left, top_left, top_right });
                    self->faces().push_back({ bottom_right, bottom_left, top_right });
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self, s32 detail_x, s32 detail_y)
        {
            f32 r_x = 1.0f;
            f32 r_y = 1.0f;
            f32 r_z = 1.0f;

            for (s32 i = 0; i <= detail_y; i++) 
            {
                f32 v = static_cast<f32>(i) / detail_y;
                f32 phi = constants::pi() * v - constants::pi() / 2;
                f32 cos_phi = std::cos(phi);
                f32 sin_phi = std::sin(phi);

                for (s32 j = 0; j <= detail_x; j++) 
                {
                    f32 u = static_cast<f32>(j) / detail_x;
                    f32 theta = 2 * constants::pi() * u;
                    f32 cos_theta = std::cos(theta);
                    f32 sin_theta = std::sin(theta);

                    // Scale the sphere's vertex to form an ellipsoid
                    self->vertex_positions().emplace_back(r_x * cos_phi * sin_theta, r_y * sin_phi, r_z * cos_phi * cos_theta);
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self, s32 detail_x, s32 detail_y)
        {
            for (s32 i = 0; i <= detail_y; i++) 
            {
                f32 v = static_cast<f32>(i) / detail_y;

                for (s32 j = 0; j <= detail_x; j++) 
                {
                    f32 u = static_cast<f32>(j) / detail_x;
                    self->vertex_uvs().emplace_back(u, v);
                }
            }
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self, s32 detail_x, s32 detail_y)
        {
            for (s32 i = 0; i <= detail_y; i++) 
            {
                f32 v = static_cast<f32>(i) / detail_y;
                f32 phi = constants::pi() * v - constants::pi() / 2;
                f32 cos_phi = std::cos(phi);
                f32 sin_phi = std::sin(phi);

                for (s32 j = 0; j <= detail_x; j++) 
                {
                    f32 u = static_cast<f32>(j) / detail_x;
                    f32 theta = 2 * constants::pi() * u;
                    f32 cos_theta = std::cos(theta);
                    f32 sin_theta = std::sin(theta);

                    // Calculate the normal by creating a unit vector
                    glm::vec3 normal(cos_phi * sin_theta, sin_phi, cos_phi * cos_theta);

                    self->vertex_normals().emplace_back(glm::normalize(normal));
                }
            }

            // Normalize each accumulated vertex normal
            for (auto& normal : self->vertex_normals())
            {
                if (glm::length(normal) > 0.0f)
                {
                    normal = glm::normalize(normal);
                }
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_sphere(bool smooth_normals, s32 detail_x, s32 detail_y)
        {
            std::stringstream stream;

            stream << "sphere|";
            stream << smooth_normals << "|";
            stream << detail_x << "|";
            stream << detail_y;

            const std::string gid = stream.str();

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [detail_x, detail_y](geometry* self)
                {
                    make_faces(self, detail_x, detail_y);

                    make_vertices(self, detail_x, detail_y);
                    make_uvs(self, detail_x, detail_y);
                    make_normals(self, detail_x, detail_y);
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