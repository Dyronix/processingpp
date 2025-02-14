#include "geometry/geometry_helpers.h"
#include "geometry/geometry.h"

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        temp_vector<glm::vec3> compute_smooth_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count)
        {
            temp_vector<glm::vec3> vertex_normals(vertex_count, glm::vec3(0.0f));

            for (u64 i = 0; i < index_count; i += 3)
            {
                u32 idx0 = indices[i];
                u32 idx1 = indices[i + 1];
                u32 idx2 = indices[i + 2];

                // Get the vertices for the current triangle
                const glm::vec3& v0 = vertices[idx0];
                const glm::vec3& v1 = vertices[idx1];
                const glm::vec3& v2 = vertices[idx2];

                // Calculate the face normal
                glm::vec3 face_normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                // Accumulate the face normal into each vertex normal
                vertex_normals[idx0] += face_normal;
                vertex_normals[idx1] += face_normal;
                vertex_normals[idx2] += face_normal;
            }

            // Normalize each accumulated vertex normal
            for (auto& normal : vertex_normals)
            {
                if (glm::length(normal) > 0.0f)
                {
                    normal = glm::normalize(normal);
                }
            }

            return vertex_normals;
        }

        //-------------------------------------------------------------------------
        temp_vector<glm::vec3> compute_flat_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count)
        {
            temp_vector<glm::vec3> flat_normals;
            flat_normals.reserve(index_count); // 1 unique normal per index

            for (u64 i = 0; i < index_count; i += 3)
            {
                u32 idx0 = indices[i];
                u32 idx1 = indices[i + 1];
                u32 idx2 = indices[i + 2];

                // Get the vertices for the current triangle
                const glm::vec3& v0 = vertices[idx0];
                const glm::vec3& v1 = vertices[idx1];
                const glm::vec3& v2 = vertices[idx2];

                // Calculate the face normal
                glm::vec3 face_normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                // Assign this face normal to each vertex of the triangle
                flat_normals.push_back(face_normal);
                flat_normals.push_back(face_normal);
                flat_normals.push_back(face_normal);
            }

            return flat_normals;
        }

        //-------------------------------------------------------------------------
        temp_vector<glm::vec3> compute_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count, bool smooth_normals)
        {
            if (smooth_normals)
            {
                return compute_smooth_normals(vertices, vertex_count, indices, index_count);
            }

            return compute_flat_normals(vertices, vertex_count, indices, index_count);
        }

        //-------------------------------------------------------------------------
        void compute_quad_faces(geometry* geom)
        {
            geom->faces().assign(2, { 0, 0, 0 });

            u64 index = 0;

            geom->faces()[index++] = { 0, 1, 2 };
            geom->faces()[index++] = { 0, 2, 3 };
        }

        //-------------------------------------------------------------------------
        void compute_quad_vertex_positions(geometry* geom)
        {
            geom->vertex_positions().assign(4, glm::vec3());

            f32 width = 1.0f;
            f32 height = 1.0f;

            u64 index = 0;

            geom->vertex_positions()[index++] = { glm::vec3(-width / 2, -height / 2, 0.0f) };
            geom->vertex_positions()[index++] = { glm::vec3(width / 2, -height / 2, 0.0f) };
            geom->vertex_positions()[index++] = { glm::vec3(width / 2, height / 2, 0.0f) };
            geom->vertex_positions()[index++] = { glm::vec3(-width / 2, height / 2, 0.0f) };
        }

        //-------------------------------------------------------------------------
        void compute_quad_vertex_uvs(geometry* geom)
        {
            geom->vertex_uvs().assign(4, glm::vec2());

            u64 index = 0;

            geom->vertex_uvs()[index++] = glm::vec2{ 0.0f, 0.0f };
            geom->vertex_uvs()[index++] = glm::vec2{ 1.0f, 0.0f };
            geom->vertex_uvs()[index++] = glm::vec2{ 1.0f, 1.0f };
            geom->vertex_uvs()[index++] = glm::vec2{ 0.0f, 1.0f };
        }

        //-------------------------------------------------------------------------
        void compute_quad_vertex_normals(geometry* geom)
        {
            geom->vertex_normals().assign(4, glm::vec3());

            u64 index = 0;

            geom->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
            geom->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
            geom->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
            geom->vertex_normals()[index++] = glm::vec3{ 0.0f, 0.0f, 1.0f };
        }
    }
}