#pragma once

#include "render/render_item_components.h"
#include "render/render_types.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <unordered_map>

namespace ppp
{
    namespace render
    {
        class instance_drawing_data
        {
        public:
            instance_drawing_data(topology_type topology, const irender_item* instance, const vertex_attribute_layout* layouts, u64 layout_count);

            void increment_instance_count();
            void append(const glm::vec4& color, const glm::mat4& world);
            void release();

            const irender_item* instance() const;
            const topology_type topology() const;

            s32 instance_count() const;

            bool has_instance_data() const;
            bool has_drawing_data() const;

            u32 vao() const;
            u32 vbo() const;
            u32 ebo() const;
            u32 ibo() const;

        private:
            u32	m_vao = 0;
            u32	m_vbo = 0;
            u32 m_ebo = 0;
            u32 m_ibo = 0;

            const irender_item* m_instance;
            const topology_type m_topology_type;

            s32 m_instance_count = 0;
            
            std::unique_ptr<class vertex_buffer> m_vertex_buffer;
            std::unique_ptr<class index_buffer> m_index_buffer;

            const vertex_attribute_layout* m_layouts;
            const u64 m_layout_count;
        };
    }
}