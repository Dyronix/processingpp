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
            self->faces().assign(4, { 0, 0, 0 });
            self->faces() = {
                { 0, 1, 2 },
                { 0, 2, 3 },
                { 0, 3, 1 },
                { 1, 3, 2 }
            };
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self)
        {
            self->vertex_positions().assign(4, glm::vec3());

            f32 w = 1.0f;
            f32 h = 1.0f;

            s32 index = 0;

            self->vertex_positions()[index++] = { glm::vec3(0, h, 0) };
            self->vertex_positions()[index++] = { glm::vec3(-w, -h, w) };
            self->vertex_positions()[index++] = { glm::vec3(w, -h, w) };
            self->vertex_positions()[index++] = { glm::vec3(0, -h, -w) };
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self)
        {
            self->vertex_uvs().assign(4, glm::vec2());

            s32 index = 0;

            self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.0f);
            self->vertex_uvs()[index++] = glm::vec2(1.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.0f, 1.0f);
            self->vertex_uvs()[index++] = glm::vec2(0.5f, 0.5f);
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {
            self->vertex_normals().assign(4, glm::vec3());
            self->compute_normals();
        }

        //-------------------------------------------------------------------------
        geometry* make_tetrahedron(bool smooth_normals)
        {
            const std::string gid = "tetrahedron|" + std::to_string(smooth_normals);

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