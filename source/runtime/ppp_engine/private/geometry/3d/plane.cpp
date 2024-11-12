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
            self->faces().assign(2, { 0, 0, 0 });

            u64 index = 0;

            self->faces()[index++] = { 0, 1, 2 };
            self->faces()[index++] = { 0, 2, 3 };
        }
        //-------------------------------------------------------------------------
        static void make_vertices(geometry* self)
        {
            self->vertex_positions().assign(4, glm::vec3());

            f32 width = 1.0f;
            f32 height = 1.0f;

            u64 index = 0;

            self->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, 0.0f) }; 
            self->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, 0.0f) };  
            self->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, 0.0f) };   
            self->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, 0.0f) };  
        }
        //-------------------------------------------------------------------------
        static void make_uvs(geometry* self)
        {
            self->vertex_uvs().assign(4, glm::vec2());

            u64 index = 0;

            self->vertex_uvs()[index++] = glm::vec2{ 0.0f, 0.0f };
            self->vertex_uvs()[index++] = glm::vec2{ 1.0f, 0.0f };
            self->vertex_uvs()[index++] = glm::vec2{ 1.0f, 1.0f };
            self->vertex_uvs()[index++] = glm::vec2{ 0.0f, 1.0f };
        }
        //-------------------------------------------------------------------------
        static void make_normals(geometry* self)
        {
            self->vertex_normals().assign(4, glm::vec3());

            u64 index = 0;

            self->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
            self->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
            self->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
            self->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
        }

        //-------------------------------------------------------------------------
        geometry* make_plane(bool smooth_normals)
        {
            const std::string gid = "plane|" + std::to_string(smooth_normals);

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