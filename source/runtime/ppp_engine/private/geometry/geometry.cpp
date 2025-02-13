#include "geometry.h"

#include "string/string_ops.h"

#include "util/log.h"

#include <glm/gtc/epsilon.hpp>

#include <string>

namespace ppp
{
    namespace geometry
    {
        static glm::vec3 get_face_normal(const graphics_vector<glm::vec3>& vertices, const graphics_vector<render::face>& faces, s32 face_id) 
        {
            auto& face = faces[face_id];

            if (face.size() != 3)
            {
                log::error("error: face {} does not have exactly 3 vertices", face_id);
                return glm::vec3(0.0f);
            }

            const glm::vec3& v_a = vertices[face[0]];
            const glm::vec3& v_b = vertices[face[1]];
            const glm::vec3& v_c = vertices[face[2]];

            glm::vec3 ab = v_b - v_a;
            glm::vec3 ac = v_c - v_a;
            glm::vec3 n = glm::cross(ab, ac);

            f32 ln = glm::length(n);
            f32 sin_alpha = ln / (glm::length(ab) * glm::length(ac));

            if (glm::epsilonEqual(sin_alpha, 0.0f, std::numeric_limits<f32>::epsilon()) || std::isnan(sin_alpha)) 
            {
                log::warn("warning: face {} has collinear sides or a repeated vertex", face_id); 
                return n;
            }

            sin_alpha = std::min(1.0f, std::max(sin_alpha, 0.0f));

            return (ln > std::numeric_limits<f32>::epsilon()) 
                ? n * (std::asin(sin_alpha) / ln) 
                : n;
        }

        //-------------------------------------------------------------------------
        geometry::geometry(std::string_view geometry_id, bool smooth_normals, const geometry_creation_fn& creation_fn)
            :m_smooth_normals(smooth_normals)
            ,m_id(std::hash<std::string_view>{}(geometry_id))
        {
            creation_fn(this);
        }

        //-------------------------------------------------------------------------
        bool geometry::has_smooth_normals() const
        {
            return m_smooth_normals;
        }

        //-------------------------------------------------------------------------
        u64 geometry::vertex_count() const
        {
            return m_vertex_positions.size();
        }

        //-------------------------------------------------------------------------
        u64 geometry::index_count() const
        {
            if (m_faces.empty())
            {
                return 0;
            }

            constexpr s32 nr_vertices_in_triangle = 3;

            return m_faces.size() * nr_vertices_in_triangle;
        }

        //-------------------------------------------------------------------------
        void geometry::compute_normals(s32 round_to_precision)
        {
            if (m_smooth_normals)
            {
                graphics_hash_map<pool_string, s32> vertex_indices;
                graphics_vector<glm::vec3> unique_vertices;

                f32 power = std::pow(10.0f, round_to_precision);

                auto rounded = [power](f32 v) { return std::round(v * power) / power; };
                auto get_key = [rounded](const glm::vec3& vert) 
                {
                    pool_stringstream stream;

                    stream << rounded(vert.x);
                    stream << ",";
                    stream << rounded(vert.y);
                    stream << ",";
                    stream << rounded(vert.z);

                    return stream.str();
                };

                // loop through each vertex and add unique_vertices
                for (const auto& v : m_vertex_positions)
                {
                    pool_string key = get_key(v);
                    if (vertex_indices.find(key) == std::cend(vertex_indices))
                    {
                        vertex_indices.emplace(key, (s32)unique_vertices.size());
                        unique_vertices.push_back(v);
                    }
                }

                // update face indices to use the deduplicated vertex indices
                for (auto& face : m_faces)
                {
                    for (render::index& fv : face)
                    {
                        pool_string key = get_key(m_vertex_positions[fv]);
                        fv = vertex_indices[key];
                    }
                }

                m_vertex_positions = unique_vertices;
            }

            m_vertex_normals.resize(m_vertex_positions.size());
            m_vertex_normals.assign(m_vertex_positions.size(), glm::vec3(0.0f, 0.0f, 0.0f));

            for (u64 f = 0; f < m_faces.size(); ++f)
            {
                glm::vec3 face_normal = get_face_normal(m_vertex_positions, m_faces, f);

                for (render::index& fv : m_faces[f])
                {
                    m_vertex_normals[fv] += face_normal;
                }
            }

            for (auto& normal : m_vertex_normals)
            {
                if (glm::length(normal) > std::numeric_limits<f32>::epsilon()) 
                {
                    normal = glm::normalize(normal);
                }
            }
        }
    }
}