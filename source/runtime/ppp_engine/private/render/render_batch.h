#pragma once

#include "render/render_item.h"
#include "render/render_types.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace ppp
{
    namespace render
    {
        /**
         * @brief Represents a batch of geometry that can be drawn together.
         *
         * Handles appending vertices, resetting, and submitting GPU data.
         */
        class batch
        {
        public:
            /**
             * @brief Constructs a batch with specified buffer sizes and attribute layouts.
             */
            batch(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count);
            ~batch();

            batch(const batch& other) = delete;             ///< Copy constructor deleted.
            batch(batch&& other) noexcept;                  ///< Move constructor.

            batch& operator=(const batch& other) = delete;  ///< Copy assignment deleted.
            batch& operator=(batch&& other) noexcept;       ///< Move assignment.

        public:
            /// @brief Binds the batch's VAO and material storage.
            void bind() const;

            /// @brief Unbinds the batch's VAO and material storage.
            void unbind() const;

            /// @brief Submits all data (vertices, indices, materials) to the GPU.
            void submit() const;

            /// @brief Draws the batch using the specified topology.
            void draw(topology_type topology) const;

        public:
            /// @brief Appends a render item to the batch.
            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            /// @brief Resets the batch (clears the buffers but keeps memory).
            void reset();

            /// @brief Frees GPU resources.
            void release();

            /// @brief Returns true if the batch can add the given number of vertices and indices.
            bool can_add(s32 nr_vertices, s32 nr_indices) const;

            /// @brief Returns true if the batch has any geometry.
            bool has_data() const;

        public:
            /// @brief Returns pointer to vertex data.
            const void* vertices() const;

            /// @brief Returns pointer to index data.
            const void* indices() const;

            /// @brief Returns the number of active vertices.
            u32 active_vertex_count() const;

            /// @brief Returns the number of active indices.
            u32 active_index_count() const;

            /// @brief Returns the size of the vertex buffer in bytes.
            u64 vertex_buffer_byte_size() const;

            /// @brief Returns the size of the index buffer in bytes.
            u64 index_buffer_byte_size() const;

            /// @brief Returns the maximum allowed vertex count.
            u32 max_vertex_count() const;

            /// @brief Returns the maximum allowed index count.
            u32 max_index_count() const;

        private:
            class impl;
            std::unique_ptr<impl> m_pimpl;
        };

        /**
         * @brief Manages multiple batches of drawing data.
         *
         * Automatically pushes new batches when a batch becomes full.
         */
        class batch_drawing_data
        {
        public:
            /**
             * @brief Constructs a batch drawing data manager.
             */
            batch_drawing_data(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count);

            ~batch_drawing_data();

            batch_drawing_data(const batch_drawing_data& other) = delete;               ///< Copying disabled.
            batch_drawing_data(batch_drawing_data&& other) noexcept;                    ///< Move constructor.

            batch_drawing_data& operator=(const batch_drawing_data& other) = delete;    ///< Copying disabled.
            batch_drawing_data& operator=(batch_drawing_data&& other) noexcept;         ///< Move assignment.

            /// @brief Appends a render item to the current batch (or creates a new batch if needed).
            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            /// @brief Resets all batches for reuse.
            void reset();

            /// @brief Releases all GPU memory used by the batches.
            void release();

            /// @brief Returns the first batch (for iteration).
            const batch* first_batch();

            /// @brief Returns the next batch (for iteration).
            const batch* next_batch();

            /// @brief Returns true if any batch has geometry.
            bool has_drawing_data() const;

        private:
            struct impl;
            std::unique_ptr<impl> m_pimpl;
        };
    }
}
