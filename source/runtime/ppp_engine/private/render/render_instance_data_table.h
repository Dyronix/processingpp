#pragma once

#include "render/render_base_renderer.h"
#include "render/render_instance.h"

namespace ppp
{
    namespace render
    {
        class instance_data_table
        {
        public:
            using table_type = std::unordered_map<topology_type, instance_drawing_data>;
            using iterator = table_type::iterator;
            using const_iterator = table_type::const_iterator;

        public:
            instance_data_table(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag);

            // Standard begin/end to allow range-based for loops.
            iterator                    begin()                         { return m_instances.begin(); }
            iterator                    end()                           { return m_instances.end(); }
            const_iterator              begin() const                   { return m_instances.begin(); }
            const_iterator              end() const                     { return m_instances.end(); }

        public:
            const table_type&           data() const                    { return m_instances; }

            const attribute_layout*     instance_layouts() const        { return m_instance_layouts; };
            u64                         instance_layout_count() const   { return m_instance_layout_count; }

        public:
            void                        reset();
            void                        clear();
            void                        append(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            u64                         size() const;
            bool                        empty() const;

            bool                        has_texture_support() const;
            bool                        has_normal_support() const;

        private:
            string::string_id           m_shader_tag;

            table_type                  m_instances;
            const attribute_layout*     m_instance_layouts;
            u64                         m_instance_layout_count;
        };
    }
}