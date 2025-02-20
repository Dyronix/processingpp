#pragma once

#include "render/render_item.h"
#include "render/render_types.h"

#include "memory/memory_types.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <unordered_map>

namespace ppp
{
    namespace render
    {
        class batch
        {
        public:
            batch(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count);
            ~batch();                                       // has to be defined, due to auto compiler generated function and forward delclaration

            batch(const batch& other) = delete;             // unique ptr, so we can delete
            batch(batch&& other) noexcept;                           // has to be defined, due to auto compiler generated function and forward delclaration

            batch& operator=(const batch& other) = delete;  // unique ptr, so we can delete
            batch& operator=(batch&& other) noexcept;                // has to be defined, due to auto compiler generated function and forward delclaration

        public:
            void bind() const;
            void unbind() const;
            void submit() const;
            void draw(topology_type topology, u32 shader_program) const;

        public:
            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world);
            void reset();
            void release();

            bool can_add(s32 nr_vertices, s32 nr_indices) const;

            bool has_data() const;

        public:
            const void* vertices() const;
            const void* indices() const;
            
            u32 active_vertex_count() const;
            u32 active_index_count() const;

            u64 vertex_buffer_byte_size() const;
            u64 index_buffer_byte_size() const;
            
            u32 max_vertex_count() const;
            u32 max_index_count() const;

        private:
            class impl;
            global_unique_ptr<impl> m_pimpl;
        };

        class batch_drawing_data
        {
        public:
            batch_drawing_data(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count, render_buffer_policy render_buffer_policy);
            ~batch_drawing_data();

            batch_drawing_data(const batch_drawing_data& other) = delete;             // unique ptr, so we can delete
            batch_drawing_data(batch_drawing_data&& other) noexcept;                           // has to be defined, due to auto compiler generated function and forward delclaration

            batch_drawing_data& operator=(const batch_drawing_data& other) = delete;  // unique ptr, so we can delete
            batch_drawing_data& operator=(batch_drawing_data&& other) noexcept;                // has to be defined, due to auto compiler generated function and forward delclaration

            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world);
            void reset();
            void release();

            const batch* first_batch();
            const batch* next_batch();

            bool has_drawing_data() const;

        private:
            using batch_arr = graphics_vector<batch>;

        private:
            struct impl;
            global_unique_ptr<impl> m_pimpl;
        };
    }
}