#pragma once

#include "render/render_batch.h"

#include "string/string_id.h"

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        class batch_data_table
        {
        public:
            using table_type = std::unordered_map<topology_type, batch_drawing_data>;
            using iterator = table_type::iterator;
            using const_iterator = table_type::const_iterator;

        public:
            batch_data_table(string::string_id shader_tag);

            // Standard begin/end to allow range-based for loops.
            iterator            begin() { return m_batches.begin(); }
            iterator            end() { return m_batches.end(); }
            const_iterator      begin() const { return m_batches.begin(); }
            const_iterator      end() const { return m_batches.end(); }

        public:
            const table_type&   data() const { return m_batches; }

        public:
            void                reset();
            void                clear();
            void                append(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            u64                 size() const;
            bool                empty() const;

            bool                has_texture_support() const;
            bool                has_normal_support() const;

        private:
            string::string_id   m_shader_tag;

            table_type          m_batches;
        };
    }
}