#include "geometry/geometry_helpers.h"

namespace ppp
{
    namespace geometry
    {
        //-------------------------------------------------------------------------
        std::vector<glm::vec3> compute_smooth_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count)
        {
            std::vector<glm::vec3> vertex_normals(vertex_count, glm::vec3(0.0f));

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
        std::vector<glm::vec3> compute_flat_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count)
        {
            std::vector<glm::vec3> flat_normals;
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
        std::vector<glm::vec3> compute_normals(const glm::vec3* vertices, u64 vertex_count, const u32* indices, u64 index_count, bool smooth_normals)
        {
            if (smooth_normals)
            {
                return compute_smooth_normals(vertices, vertex_count, indices, index_count);
            }

            return compute_flat_normals(vertices, vertex_count, indices, index_count);
        }
    }
}