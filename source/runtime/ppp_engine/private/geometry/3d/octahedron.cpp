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
            // Allocate 8 faces (each face is a triangle defined by 3 vertex indices)
            self->faces().assign(8, { 0, 0, 0 });
            self->faces() = {
                { 0,  1,  2  },  // Face 0: (A, C, B)
                { 3,  4,  5  },  // Face 1: (A, D, C)
                { 6,  7,  8  },  // Face 2: (A, E, D)
                { 9, 10, 11  },  // Face 3: (A, B, E)
                { 12, 13, 14 },  // Face 4: (F, B, C)
                { 15, 16, 17 },  // Face 5: (F, C, D)
                { 18, 19, 20 },  // Face 6: (F, D, E)
                { 21, 22, 23 }   // Face 7: (F, E, B)
            };
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self)
        {
            // Allocate space for 24 vertices (3 per face for 8 faces)
            self->vertex_positions().assign(24, glm::vec3());

            f32 w = 1.0f;
            f32 h = 1.0f;
            s32 index = 0;

            // Define the unique octahedron vertices:
            // A: Top, B: Right, C: Front, D: Left, E: Back, F: Bottom.
            glm::vec3 A(0, h, 0);   // Top
            glm::vec3 B(w, 0, 0);   // Right
            glm::vec3 C(0, 0, w);   // Front
            glm::vec3 D(-w, 0, 0);  // Left
            glm::vec3 E(0, 0, -w);  // Back
            glm::vec3 F(0, -h, 0);  // Bottom

            // Upper faces (using top vertex A)
            // Face 0: (A, C, B) � corresponds to original {0,2,1}
            self->vertex_positions()[index++] = A;
            self->vertex_positions()[index++] = C;
            self->vertex_positions()[index++] = B;

            // Face 1: (A, D, C) � corresponds to original {0,3,2}
            self->vertex_positions()[index++] = A;
            self->vertex_positions()[index++] = D;
            self->vertex_positions()[index++] = C;

            // Face 2: (A, E, D) � corresponds to original {0,4,3}
            self->vertex_positions()[index++] = A;
            self->vertex_positions()[index++] = E;
            self->vertex_positions()[index++] = D;

            // Face 3: (A, B, E) � corresponds to original {0,1,4}
            self->vertex_positions()[index++] = A;
            self->vertex_positions()[index++] = B;
            self->vertex_positions()[index++] = E;

            // Lower faces (using bottom vertex F)
            // Face 4: (F, B, C) � corresponds to original {5,1,2}
            self->vertex_positions()[index++] = F;
            self->vertex_positions()[index++] = B;
            self->vertex_positions()[index++] = C;

            // Face 5: (F, C, D) � corresponds to original {5,2,3}
            self->vertex_positions()[index++] = F;
            self->vertex_positions()[index++] = C;
            self->vertex_positions()[index++] = D;

            // Face 6: (F, D, E) � corresponds to original {5,3,4}
            self->vertex_positions()[index++] = F;
            self->vertex_positions()[index++] = D;
            self->vertex_positions()[index++] = E;

            // Face 7: (F, E, B) � corresponds to original {5,4,1}
            self->vertex_positions()[index++] = F;
            self->vertex_positions()[index++] = E;
            self->vertex_positions()[index++] = B;
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self)
        {
            // Allocate space for 24 UVs (3 per face for 8 faces)
            self->vertex_uvs().assign(24, glm::vec2());
            s32 index = 0;

            // For each face, we assign a common set of UVs.
            // You might want to customize these for your texture mapping.
            for (int i = 0; i < 8; i++) {
                self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.0f);
                self->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
                self->vertex_uvs()[index++] = glm::vec2(0.0f, 1.0f);
            }
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {
            // Allocate space for 24 normals
            self->vertex_normals().assign(24, glm::vec3());

            // For each face, compute the face normal and assign it to all three vertices.
            for (size_t i = 0; i < self->faces().size(); i++) {
                auto face = self->faces()[i];

                // Retrieve the vertices of the face
                glm::vec3 a = self->vertex_positions()[face[0]];
                glm::vec3 b = self->vertex_positions()[face[1]];
                glm::vec3 c = self->vertex_positions()[face[2]];

                // Calculate the face normal using the cross product
                glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));

                // Assign the computed normal to each vertex of the face
                self->vertex_normals()[face[0]] = normal;
                self->vertex_normals()[face[1]] = normal;
                self->vertex_normals()[face[2]] = normal;
            }
        }

        //-------------------------------------------------------------------------
        geometry* make_octahedron(bool smooth_normals)
        {
            temp_stringstream stream;

            stream << "octahedron|";
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