#include "geometry/3d/box.h"

#include "resources/geometry_pool.h"

#include <array>
#include <sstream>

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        static void make_faces(geometry* self)
        {
            // Allocate 4 faces referencing the 12 vertices (indices 0 to 11)
            self->faces().assign(4, { 0, 0, 0 });
            self->faces() = {
                { 0, 1, 2 },    // Face 0: vertices A, B, C
                { 3, 4, 5 },    // Face 1: vertices A, C, D
                { 6, 7, 8 },    // Face 2: vertices A, D, B
                { 9, 10, 11 }   // Face 3: vertices B, D, C
            };
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self)
        {
            // Allocate space for 12 vertices (3 vertices per face for 4 faces)
            self->vertex_positions().assign(12, glm::vec3());

            f32 w = 1.0f;
            f32 h = 1.0f;

            s32 index = 0;

            // Define the original tetrahedron corners.
            glm::vec3 A(0, h, 0);
            glm::vec3 B(-w, -h, w);
            glm::vec3 C(w, -h, w);
            glm::vec3 D(0, -h, -w);

            // Face 0: vertices A, B, C
            self->vertex_positions()[index++] = A;
            self->vertex_positions()[index++] = B;
            self->vertex_positions()[index++] = C;

            // Face 1: vertices A, C, D
            self->vertex_positions()[index++] = A;
            self->vertex_positions()[index++] = C;
            self->vertex_positions()[index++] = D;

            // Face 2: vertices A, D, B
            self->vertex_positions()[index++] = A;
            self->vertex_positions()[index++] = D;
            self->vertex_positions()[index++] = B;

            // Face 3: vertices B, D, C
            self->vertex_positions()[index++] = B;
            self->vertex_positions()[index++] = D;
            self->vertex_positions()[index++] = C;
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self)
        {
            // Allocate space for 12 UVs (3 per face for 4 faces)
            self->vertex_uvs().assign(12, glm::vec2());
            s32 index = 0;

            // Face 0: UVs for vertices A, B, C
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.0f);
            self->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.0f, 1.0f);

            // Face 1: UVs for vertices A, C, D
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.0f);
            self->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.0f, 1.0f);

            // Face 2: UVs for vertices A, D, B
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.0f);
            self->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.0f, 1.0f);

            // Face 3: UVs for vertices B, D, C
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.0f);
            self->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.0f, 1.0f);
        }

        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {
            // 1) Initialize the normals array to all zeros
            self->vertex_normals().assign(self->vertex_positions().size(), glm::vec3(0.0f));

            // 2) Accumulate face normals into each vertex
            for (auto& face : self->faces())
            {
                const glm::vec3& a = self->vertex_positions()[face[0]];
                const glm::vec3& b = self->vertex_positions()[face[1]];
                const glm::vec3& c = self->vertex_positions()[face[2]];

                // Two edges of the face
                glm::vec3 edge1 = b - a;
                glm::vec3 edge2 = c - a;

                // Face normal
                glm::vec3 face_normal = glm::normalize(glm::cross(edge1, edge2));

                // Add the face normal to each of the face's vertices
                self->vertex_normals()[face[0]] += face_normal;
                self->vertex_normals()[face[1]] += face_normal;
                self->vertex_normals()[face[2]] += face_normal;
            }

            // 3) Normalize each accumulated vertex normal
            for (auto& normal : self->vertex_normals())
            {
                normal = glm::normalize(normal);
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_tetrahedron(bool smooth_normals)
        {
            temp_stringstream stream;

            stream << "tetrahedron|";
            stream << smooth_normals;

            const temp_string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry* self)
                {
                    make_faces(self);

                    make_vertices(self);
                    make_uvs(self);
                    make_normals(self);
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