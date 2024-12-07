#pragma once

#include "util/types.h"

#include "render/render_item_components.h"

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <functional>

namespace ppp
{
    namespace geometry
    {
        using geometry_creation_fn = std::function<void(class geometry*)>;

        class geometry : public render::irender_item
        {
        public:
            geometry(const std::string& id, bool smooth_normals, const geometry_creation_fn& creation_fn);

            u64 vertex_count() const override;
            u64 index_count() const override;
            u64 texture_count() const override;

            void compute_normals(s32 round_to_precision = 3);

            const std::vector<glm::vec3>& vertex_positions() const override { return m_vertex_positions; }
            const std::vector<glm::vec3>& vertex_normals() const override { return m_vertex_normals; }
            const std::vector<glm::vec2>& vertex_uvs() const override { return m_vertex_uvs; }

            const std::vector<render::face>& faces() const override { return m_faces; }
            const std::vector<render::texture_id>& texture_ids() const override { return m_texture_ids; }

            const u64 id() const override { return m_id; }
            
            std::vector<glm::vec3>& vertex_positions() { return m_vertex_positions; }
            std::vector<glm::vec3>& vertex_normals() { return m_vertex_normals; }
            std::vector<glm::vec2>& vertex_uvs() { return m_vertex_uvs; }

            std::vector<render::face>& faces() { return m_faces; }
            std::vector<render::texture_id>& texture_ids() { return m_texture_ids; }

        private:
            std::vector<glm::vec3> m_vertex_positions;
            std::vector<glm::vec3> m_vertex_normals;
            std::vector<glm::vec2> m_vertex_uvs;
            std::vector<glm::vec4> m_vertex_colors;

            std::vector<render::face> m_faces;
            std::vector<render::texture_id> m_texture_ids;

            bool m_smooth_normals; 

            u64 m_id;
        };
    }
}