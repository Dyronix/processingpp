#pragma once

#include "util/types.h"
#include "geometry/geometry_bounding_box.h"

#include "memory/memory_types.h"

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <functional>

namespace ppp
{
    namespace geometry
    {
        using geometry_creation_fn = std::function<void(class geometry*)>;

        class geometry
        {
        public:
            geometry(std::string_view id, bool smooth_normals, const geometry_creation_fn& creation_fn);

            bool has_smooth_normals() const;

            u64 vertex_count() const;
            u64 index_count() const;

            void compute_normals(s32 round_to_precision = 3);
            void compute_aabb();

            const graphics_vector<glm::vec3>& vertex_positions() const { return m_vertex_positions; }
            const graphics_vector<glm::vec3>& vertex_normals() const { return m_vertex_normals; }
            const graphics_vector<glm::vec2>& vertex_uvs() const { return m_vertex_uvs; }

            const graphics_vector<render::face>& faces() const { return m_faces; }

            const u64 id() const { return m_id; }
            
            graphics_vector<glm::vec3>& vertex_positions() { return m_vertex_positions; }
            graphics_vector<glm::vec3>& vertex_normals() { return m_vertex_normals; }
            graphics_vector<glm::vec2>& vertex_uvs() { return m_vertex_uvs; }

            graphics_vector<render::face>& faces() { return m_faces; }

            const bounding_box& aabb();

        private:
            graphics_vector<glm::vec3> m_vertex_positions;
            graphics_vector<glm::vec3> m_vertex_normals;
            graphics_vector<glm::vec2> m_vertex_uvs;
            graphics_vector<glm::vec4> m_vertex_colors;
            
            graphics_vector<render::face> m_faces;
            bounding_box m_bounding_box;

        private:
            bool m_smooth_normals; 

            u64 m_id;
        };
    }
}