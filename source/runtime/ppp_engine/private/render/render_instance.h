#pragma once

#include "render/render_item.h"
#include "render/render_index_buffer.h"
#include "render/render_types.h"

#include <glm/glm.hpp>

#include <memory>

namespace ppp
{
    namespace render
    {
        /**
         * @brief Represents a single instanced render-able object.
         */
        class instance
        {
        public:
            /**
             * @brief Constructs an instance from a base render item and vertex layouts.
             */
            instance(const irender_item* instance, const attribute_layout* layouts, u32 layout_count);

            ~instance();

            instance(const instance& other) = delete;             ///< Copying is disabled (unique_ptr).
            instance(instance&& other) noexcept;                  ///< Move constructor.

            instance& operator=(const instance& other) = delete;  ///< Copying is disabled (unique_ptr).
            instance& operator=(instance&& other) noexcept;       ///< Move assignment.

        public:
            /**
             * @brief Binds the instance for rendering.
             */
            void bind() const;

            /**
             * @brief Unbinds the instance after rendering.
             */
            void unbind() const;

            /**
             * @brief Submits GPU data (vertex/index/instance data).
             */
            void submit() const;

            /**
             * @brief Draws the instance with the given topology type.
             */
            void draw(topology_type topology) const;

        public:
            /**
             * @brief Increments the number of active instances.
             */
            void increment_instance_count() const;

            /**
             * @brief Appends a new instance transformation and color.
             */
            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world) const;

            /**
             * @brief Resets instance data (preserves vertex/index buffer).
             */
            void reset() const;

            /**
             * @brief Releases all associated GPU resources.
             */
            void release() const;

            /**
             * @brief Returns true if there is any drawing data.
             */
            bool has_data() const;

        public:
            /// @brief Returns the instance ID (geometry ID).
            u64 instance_id() const;

            /// @brief Returns a pointer to vertex data.
            const void* vertices() const;

            /// @brief Returns a pointer to index data.
            const void* indices() const;

            /// @brief Returns the number of active vertices.
            u32 active_vertex_count() const;

            /// @brief Returns the number of active indices.
            u32 active_index_count() const;

            /// @brief Returns the total size of the vertex buffer in bytes.
            u64 vertex_buffer_byte_size() const;

            /// @brief Returns the total size of the index buffer in bytes.
            u64 index_buffer_byte_size() const;

        private:
            class impl;
            std::unique_ptr<impl> m_pimpl;
        };

        /**
         * @brief Holds multiple instances of a geometry for drawing.
         */
        class instance_drawing_data
        {
        public:
            /**
             * @brief Constructs instance drawing data with vertex and instance layouts.
             */
            instance_drawing_data(const attribute_layout* layouts, u32 layout_count);

            ~instance_drawing_data();

            instance_drawing_data(const instance_drawing_data& other) = delete;             ///< Copying disabled.
            instance_drawing_data(instance_drawing_data&& other) noexcept;                  ///< Move constructor.

            instance_drawing_data& operator=(const instance_drawing_data& other) = delete;  ///< Copying disabled.
            instance_drawing_data& operator=(instance_drawing_data&& other) noexcept;       ///< Move assignment.

        public:
            /**
             * @brief Appends a new item to the instance drawing data.
             */
            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world) const;

            /**
             * @brief Resets all internal instances.
             */
            void reset() const;

            /**
             * @brief Releases all internal resources.
             */
            void release() const;

            /**
             * @brief Returns the first instance (for iteration).
             */
            const instance* first_instance() const;

            /**
             * @brief Returns the next instance (for iteration).
             */
            const instance* next_instance() const;

            /**
             * @brief Returns true if there is any drawing data.
             */
            bool has_drawing_data() const;

        private:
            struct impl;
            std::unique_ptr<impl> m_pimpl;
        };
    }
}