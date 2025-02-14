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
            self->faces().assign(12, { 0, 0, 0 });
            self->faces() = {
                // Front face
                {0, 1, 2}, {0, 2, 3},
                // Back face
                {4, 6, 5}, {4, 7, 6},
                // Left face
                {8, 9, 10}, {8, 10, 11},
                // Right face
                {12, 14, 13}, {12, 15, 14},
                // Bottom face
                {16, 18, 17}, {16, 19, 18},
                // Top face
                {20, 21, 22}, {20, 22, 23}
            };
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self)
        {
            self->vertex_positions().assign(24, glm::vec3());

            f32 width = 1.0f;
            f32 height = 1.0f;
            f32 depth = 1.0f;

            u64 index = 0;

            // Front face
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) }; // Bottom Left
            self->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };  // Bottom Right
            self->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };   // Top Right
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };  // Top Left

            // Back face
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Bottom Left
            self->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Bottom Right
            self->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Top Right
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Top Left

            // Left face
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Bottom Back
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) };  // Bottom Front
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };   // Top Front
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Top Back

            // Right face
            self->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Bottom Back
            self->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };   // Bottom Front
            self->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };    // Top Front
            self->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Top Back

            // Top face
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Back Left
            self->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Back Right
            self->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };    // Front Right
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };   // Front Left

            // Bottom face
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Back Left
            self->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Back Right
            self->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };   // Front Right
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) };  // Front Left
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self)
        {
            self->vertex_uvs().assign(24, glm::vec2());
            self->vertex_uvs() =
            {
                // Front face (matches vertices 0, 1, 2, 3)
                glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                // Back face (matches vertices 4, 5, 6, 7)
                glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                // Left face (matches vertices 8, 9, 10, 11)
                glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                // Right face (matches vertices 12, 13, 14, 15)
                glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                // Bottom face (matches vertices 16, 17, 18, 19)
                glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                // Top face (matches vertices 20, 21, 22, 23)
                glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f}
            };
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {
            self->vertex_normals().assign(24, glm::vec3());
            self->compute_normals();
        }

        //-------------------------------------------------------------------------
        geometry* make_box(bool smooth_normals)
        {
            temp_stringstream stream;

            stream << "box|";
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