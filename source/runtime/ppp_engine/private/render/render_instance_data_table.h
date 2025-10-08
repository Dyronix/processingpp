#pragma once

#include "render/render_instance.h"

#include "string/string_id.h"

#include "util/types.h"

#include <unordered_map>

namespace ppp
{
    namespace render
    {
        /**
         * @brief Manages a table of instance drawing data organized by topology type.
         *
         * Allows appending render items for instanced rendering, resetting, and querying instance data.
         */
        class instance_data_table
        {
        public:
            using table_type = std::unordered_map<topology_type, instance_drawing_data>;
            using iterator = table_type::iterator;
            using const_iterator = table_type::const_iterator;

        public:
            /**
             * @brief Constructs an instance data table.
             *
             * @param shader_tag Shader identifier associated with the table.
             */
            instance_data_table(string::string_id shader_tag);

            /// @brief Returns an iterator to the beginning of the instance data.
            iterator begin() { return m_instances.begin(); }
            /// @brief Returns an iterator to the end of the instance data.
            iterator end() { return m_instances.end(); }
            /// @brief Returns a const iterator to the beginning of the instance data.
            const_iterator begin() const { return m_instances.begin(); }
            /// @brief Returns a const iterator to the end of the instance data.
            const_iterator end() const { return m_instances.end(); }

            /**
             * @brief Returns a const reference to the internal instance data.
             */
            const table_type& data() const { return m_instances; }

        public:
            /**
             * @brief Resets all instance drawing data without clearing the table.
             */
            void reset();

            /**
             * @brief Clears all instance data and releases associated memory.
             */
            void clear();

            /**
             * @brief Appends a render item into the appropriate instance batch based on topology.
             *
             * @param topology The primitive topology of the item.
             * @param item Pointer to the render item.
             * @param color Vertex color to apply.
             * @param world World transformation matrix.
             */
            void append(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            /**
             * @brief Returns the number of instance batches.
             */
            u64 size() const;

            /**
             * @brief Checks if the instance data table is empty.
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
            string::string_id           m_shader_tag;               ///< Identifier for the shader program.
            table_type                  m_instances;                ///< Storage for instance batches keyed by topology type.
        };
    }
}