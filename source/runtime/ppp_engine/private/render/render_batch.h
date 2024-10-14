#pragma once

#include "render/render_item_components.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <unordered_map>

namespace ppp
{
    namespace render
    {
        class batch_buffer_manager;
        class batch_texture_manager;

        class batch
        {
        public:
            batch(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count, s32 size_textures = -1);
            ~batch();                                       // has to be defined, due to auto compiler generated function and forward delclaration

            batch(const batch& other) = delete;             // unique ptr, so we can delete
            batch(batch&& other);                           // has to be defined, due to auto compiler generated function and forward delclaration

            batch& operator=(const batch& other) = delete;  // unique ptr, so we can delete
            batch& operator=(batch&& other);                // has to be defined, due to auto compiler generated function and forward delclaration

            void append(const render_item& item, const glm::vec4& color, const glm::mat4& world);
            void reset();

            bool can_add(s32 nr_vertices, s32 nr_indices) const;
            bool has_reserved_texture_space() const;

            const void* vertices() const;
            const void* indices() const;
            const s32* samplers() const;
            const u32* textures() const;
            
            u32 active_vertex_count() const;
            u32 active_index_count() const;
            u32 active_sampler_count() const;
            u32 active_texture_count() const;

            u64 vertex_buffer_byte_size() const;
            u64 index_buffer_byte_size() const;
            
            u32 max_vertex_count() const;
            u32 max_index_count() const;
            u32 max_texture_count() const;

        private:
            std::unique_ptr<batch_buffer_manager> m_buffer_manager;
            std::unique_ptr<batch_texture_manager> m_texture_manager;
        };

        class batch_drawing_data
        {
        public:
            batch_drawing_data(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count, s32 size_textures = -1);

            void append(const render_item& item, const glm::vec4& color, const glm::mat4& world);
            void reset();
            void release();
            void load_first_batch();

            const batch* next_batch();

            s32 batch_count() const;

            u32 vao() const;
            u32 vbo() const;
            u32 ebo() const;

        private:
            using batch_arr = std::vector<batch>;

            u32	m_vao = 0;
            u32	m_vbo = 0;
            u32 m_ebo = 0;

            s32 m_draw_batch = 0;
            s32 m_push_batch = 0;

            batch_arr m_batches;

            const vertex_attribute_layout* m_layouts;
            const u64 m_layout_count;
        };
    }
}