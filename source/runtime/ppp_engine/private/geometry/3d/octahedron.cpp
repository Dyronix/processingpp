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
            self->faces().assign(8, { 0, 0, 0 });
            self->faces() = {
                { 0, 2, 1 },
                { 0, 3, 2 },
                { 0, 4, 3 },
                { 0, 1, 4 },  

                { 5, 1, 2 },
                { 5, 2, 3 },
                { 5, 3, 4 },
                { 5, 4, 1 }
            };
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self)
        {
            self->vertex_positions().assign(6, glm::vec3());

            f32 w = 1.0f;
            f32 h = 1.0f;

            s32 index = 0;

            self->vertex_positions()[index++] = { glm::vec3(0, h, 0) };
            self->vertex_positions()[index++] = { glm::vec3(w, 0, 0) };
            self->vertex_positions()[index++] = { glm::vec3(0, 0, w) };
            self->vertex_positions()[index++] = { glm::vec3(-w, 0, 0) };
            self->vertex_positions()[index++] = { glm::vec3(0, 0, -w) };
            self->vertex_positions()[index++] = { glm::vec3(0, -h, 0) };
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self)
        {
            self->vertex_uvs().assign(6, glm::vec2());

            s32 index = 0;

            self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.0f);
            self->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 1.0f);
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {
            self->vertex_normals().assign(6, glm::vec3());
            self->compute_normals();
        }

        //-------------------------------------------------------------------------
        geometry* make_octahedron(bool smooth_normals)
        {
            const std::string gid = "octahedron|" + std::to_string(smooth_normals);

            const geometry* geom = nullptr;

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry* self)
                {
                    make_faces(self);

                    make_vertices(self);
                    make_uvs(self);
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