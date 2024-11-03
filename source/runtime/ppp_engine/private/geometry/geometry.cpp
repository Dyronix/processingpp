#include "geometry.h"

#include "util/log.h"

#include <glm/gtc/epsilon.hpp>

#include <string>

namespace ppp
{
    namespace geometry
    {
        static glm::vec3 get_face_normal(const std::vector<glm::vec3>& vertices, const std::vector<render::face>& faces, s32 face_id) 
        {
            if (faces.size() != 3) 
            {
                log::error("error: face {} does not have exactly 3 vertices", face_id); 
                return glm::vec3(0.0f); 
            }

            auto& face = faces[face_id];

            const glm::vec3& v_a = vertices[face.fvs[0]];
            const glm::vec3& v_b = vertices[face.fvs[1]];
            const glm::vec3& v_c = vertices[face.fvs[2]];

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
        geometry::geometry(bool smooth_normals, const geometry_creation_fn& creation_fn)
            :m_smooth_normals(smooth_normals)
        {
            creation_fn(this);
        }

        //-------------------------------------------------------------------------
        u64 geometry::vertex_count() const
        {
            if (m_faces.empty())
            {
                return m_vertex_positions.size();
            }

            constexpr s32 nr_vertices_in_triangle = 3;

            return m_faces.size() * nr_vertices_in_triangle;
        }

        //-------------------------------------------------------------------------
        u64 geometry::index_count() const
        {
            return m_faces.size();
        }

        //-------------------------------------------------------------------------
        void geometry::compute_normals(s32 round_to_precision)
        {
            if (m_smooth_normals)
            {
                std::unordered_map<std::string, s32> vertex_indices;
                std::vector<glm::vec3> unique_vertices;

                f32 power = std::pow(10.0f, round_to_precision);

                auto rounded = [power](f32 v) { return std::round(v * power) / power; };
                auto get_key = [rounded](const glm::vec3& vert) 
                {
                    return std::to_string(rounded(vert.x)) + "," + std::to_string(rounded(vert.y)) + "," + std::to_string(rounded(vert.z));
                };

                for (const auto& v : m_vertex_positions)
                {
                    std::string key = get_key(v);
                    if (vertex_indices.find(key) == std::cend(vertex_indices))
                    {
                        vertex_indices.emplace(key, (s32)unique_vertices.size());
                        unique_vertices.push_back(v);
                    }
                }

                for (auto& face : m_faces)
                {
                    for (render::index& fv : face.fvs)
                    {
                        std::string key = get_key(m_vertex_positions[fv]);
                        fv = vertex_indices[key];
                    }
                }

                m_vertex_positions = unique_vertices;
            }

            for (u64 f = 0; f < m_faces.size(); ++f)
            {
                glm::vec3 face_normal = get_face_normal(m_vertex_positions, m_faces, f);

                for (s32 fv : m_faces[f].fvs)
                {
                    m_vertex_normals[fv] += face_normal;
                }
            }

            m_vertex_normals.assign(m_vertex_positions.size(), glm::vec3(0.0f, 0.0f, 0.0f));

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