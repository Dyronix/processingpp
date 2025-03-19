#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <functional>
#include <string>

namespace ppp
{
    namespace geometry
    {
        using geometry_creation_fn = std::function<void(class geometry*)>;

        struct bounding_box
        {
            glm::vec3 min;
            glm::vec3 max;
            glm::vec3 size;
            glm::vec3 offset;

            explicit operator bool() const
            {
                return glm::length(size) > 0;
            }
        };

        class geometry
        {
        public:
            geometry(std::string_view id, bool smooth_normals, const geometry_creation_fn& creation_fn);

            bool has_smooth_normals() const;

            u64 vertex_count() const;
            u64 index_count() const;

            void compute_normals(s32 round_to_precision = 3);
            void compute_aabb();

            const std::vector<glm::vec3>& vertex_positions() const { return m_vertex_positions; }
            const std::vector<glm::vec3>& vertex_normals() const { return m_vertex_normals; }
            const std::vector<glm::vec2>& vertex_uvs() const { return m_vertex_uvs; }

            const std::vector<render::face>& faces() const { return m_faces; }

            const u64 id() const { return m_id; }
            
            std::vector<glm::vec3>& vertex_positions() { return m_vertex_positions; }
            std::vector<glm::vec3>& vertex_normals() { return m_vertex_normals; }
            std::vector<glm::vec2>& vertex_uvs() { return m_vertex_uvs; }

            std::vector<render::face>& faces() { return m_faces; }

            const bounding_box& bounding_box() { if (!m_bounding_box) compute_aabb(); return m_bounding_box; }

        private:
            std::vector<glm::vec3> m_vertex_positions;
            std::vector<glm::vec3> m_vertex_normals;
            std::vector<glm::vec2> m_vertex_uvs;
            std::vector<glm::vec4> m_vertex_colors;
            
            std::vector<render::face> m_faces;

            bounding_box m_bounding_box;

        private:
            bool m_smooth_normals; 

            u64 m_id;
        };
    }
}