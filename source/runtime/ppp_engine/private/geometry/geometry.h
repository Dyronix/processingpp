#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <functional>

namespace ppp
{
    namespace geometry
    {
        using geometry_creation_fn = std::function<void(class geometry*)>;

        struct face
        {
            std::array<render::index, 3> fvs;
        };

        class geometry
        {
        public:
            geometry(bool smooth_normals, const geometry_creation_fn& creation_fn);

            u64 vertex_count() const;
            u64 index_count() const;

            void compute_normals(s32 round_to_precision = 3);

            const std::vector<glm::vec3>& vertex_positions() const { return m_vertex_positions; }
            const std::vector<glm::vec3>& vertex_normals() const { return m_vertex_normals; }
            const std::vector<glm::vec2>& vertex_uvs() const { return m_vertex_uvs; }
            const std::vector<face>& faces() const { return m_faces; }
            
            std::vector<glm::vec3>& vertex_positions() { return m_vertex_positions; }
            std::vector<glm::vec3>& vertex_normals() { return m_vertex_normals; }
            std::vector<glm::vec2>& vertex_uvs() { return m_vertex_uvs; }
            std::vector<face>& faces() { return m_faces; }

        private:
            std::vector<glm::vec3> m_vertex_positions;
            std::vector<glm::vec3> m_vertex_normals;
            std::vector<glm::vec2> m_vertex_uvs;

            std::vector<face> m_faces;

            bool m_smooth_normals;
        };
    }
}