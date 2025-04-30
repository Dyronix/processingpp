#pragma once

#include "render/render_batch.h"

#include "string/string_id.h"

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        /**
         * @brief Manages a table of batch drawing data organized by topology type.
         *
         * Allows appending render items, resetting, and querying batch data for rendering.
         */
        class batch_data_table
        {
        public:
            using table_type = std::unordered_map<topology_type, batch_drawing_data>;
            using iterator = table_type::iterator;
            using const_iterator = table_type::const_iterator;

        public:
            /**
             * @brief Constructs a batch data table associated with a specific shader.
             * @param shader_tag Identifier for the shader program.
             */
            batch_data_table(string::string_id shader_tag);

            /// @brief Returns an iterator to the beginning of the batch data.
            iterator begin() { return m_batches.begin(); }
            /// @brief Returns an iterator to the end of the batch data.
            iterator end() { return m_batches.end(); }
            /// @brief Returns a const iterator to the beginning of the batch data.
            const_iterator begin() const { return m_batches.begin(); }
            /// @brief Returns a const iterator to the end of the batch data.
            const_iterator end() const { return m_batches.end(); }

            /**
             * @brief Returns a const reference to the internal batch data.
             */
            const table_type& data() const { return m_batches; }

            /**
             * @brief Resets all batch drawing data without clearing the table.
             */
            void reset();

            /**
             * @brief Clears all batch data and releases memory.
             */
            void clear();

            /**
             * @brief Appends a render item into the appropriate batch based on topology.
             * @param topology The primitive topology of the item.
             * @param item Pointer to the render item.
             * @param color Vertex color to apply.
             * @param world World transformation matrix.
             */
            void append(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            /**
             * @brief Returns the number of batches.
             */
            u64 size() const;

            /**
             * @brief Checks if the batch data is empty.
             */
            bool empty() const;

            /**
             * @brief Checks if the associated shader supports texture coordinates.
             */
            bool has_texture_support() const;

            /**
             * @brief Checks if the associated shader supports normals.
             */
            bool has_normal_support() const;

        private:
            string::string_id m_shader_tag;     ///< Identifier for the shader associated with this batch table.
            table_type m_batches;               ///< Storage for batches keyed by topology type.
        };
    }
}