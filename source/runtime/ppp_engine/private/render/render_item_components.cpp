#include "render/render_item_components.h"

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static s32 size_in_bytes_for_data_type(vertex_attribute_data_type type)
            {
                switch (type)
                {
                case vertex_attribute_data_type::FLOAT: return 4;
                case vertex_attribute_data_type::UNSIGNED_INT: return 4;
                }
                return 0;  // Fallback to avoid compiler warnings
            }

            //-------------------------------------------------------------------------
            static s32 component_count_for_vertex_attribute(vertex_attribute_type type)
            {
                switch (type)
                {
                case vertex_attribute_type::POSITION:       return 3;
                case vertex_attribute_type::NORMAL:         return 3;
                case vertex_attribute_type::TEXCOORD:       return 2;
                case vertex_attribute_type::COLOR:          return 4;
                case vertex_attribute_type::TEXTURE_INDEX:  return 1;
                }
                return 0;  // Fallback to avoid compiler warnings
            }

            //-------------------------------------------------------------------------
            static u64 calculate_attribute_type_size_in_bytes(const vertex_attribute_map& attributes)
            {
                u64 total = 0;
                for (const auto& pair : attributes)
                {
                    s32 component_count = component_count_for_vertex_attribute(pair.second.layout.type);
                    s32 size_in_bytes = size_in_bytes_for_data_type(pair.second.layout.data_type);

                    total += (size_in_bytes * component_count);
                }
                return total;
            }
        }

        // Vertex Component
        //-------------------------------------------------------------------------
        vertex_component::vertex_component(u64 vertex_count)
            : m_vertex_count(vertex_count)
        {}

        //-------------------------------------------------------------------------
        void vertex_component::add_attribute(vertex_attribute_type type, glm::vec2* data)
        {
            add_attribute(type, vertex_attribute_data_type::FLOAT, data);
        }

        //-------------------------------------------------------------------------
        void vertex_component::add_attribute(vertex_attribute_type type, glm::vec3* data)
        {
            add_attribute(type, vertex_attribute_data_type::FLOAT, data);
        }

        //-------------------------------------------------------------------------
        void vertex_component::add_attribute(vertex_attribute_type type, f32* data)
        {
            add_attribute(type, vertex_attribute_data_type::FLOAT, data);
        }

        //-------------------------------------------------------------------------
        void vertex_component::add_attribute(vertex_attribute_type type, glm::uvec2* data)
        {
            add_attribute(type, vertex_attribute_data_type::UNSIGNED_INT, data);
        }

        //-------------------------------------------------------------------------
        void vertex_component::add_attribute(vertex_attribute_type type, glm::uvec3* data)
        {
            add_attribute(type, vertex_attribute_data_type::UNSIGNED_INT, data);
        }

        //-------------------------------------------------------------------------
        void vertex_component::add_attribute(vertex_attribute_type type, u32* data)
        {
            add_attribute(type, vertex_attribute_data_type::UNSIGNED_INT, data);
        }

        //-------------------------------------------------------------------------
        u64 vertex_component::vertex_count() const
        {
            return m_vertex_count;
        }

        //-------------------------------------------------------------------------
        void vertex_component::add_attribute(vertex_attribute_type type, vertex_attribute_data_type data_type, void* data)
        {
            auto it = m_attributes.find(type);
            if (it == std::cend(m_attributes))
            {
                s32 layout_idx = m_attributes.size(); // layout idx will change once we assign the first value
                s32 component_count = internal::component_count_for_vertex_attribute(type);
                s32 total_data_type_size_in_bytes = internal::size_in_bytes_for_data_type(data_type);
                u64 total_attrib_size_in_bytes = internal::calculate_attribute_type_size_in_bytes(m_attributes);

                // Initialize layout attributes
                m_attributes[type].layout.idx = layout_idx;
                m_attributes[type].layout.count = component_count;
                m_attributes[type].layout.type = type;
                m_attributes[type].layout.data_type = data_type;
                m_attributes[type].layout.normalized = false;
                m_attributes[type].layout.stride = total_attrib_size_in_bytes;
                m_attributes[type].layout.offset = total_attrib_size_in_bytes * layout_idx;

                // Calculate the total size needed and resize the blob
                u64 total_size = (total_data_type_size_in_bytes * component_count) * m_vertex_count;
                m_attributes[type].blob.resize(total_size);

                // Copy data into the vector's memory
                memcpy(m_attributes[type].blob.data(), data, total_size);
            }

            evaluate_attributes();
        }

        //-------------------------------------------------------------------------
        void vertex_component::evaluate_attributes()
        {
            u64 total_attrib_size_in_bytes = internal::calculate_attribute_type_size_in_bytes(m_attributes);

            for(auto& pair : m_attributes)
            {
                s32 component_count = internal::component_count_for_vertex_attribute(pair.second.layout.type);
                s32 size_in_bytes = internal::size_in_bytes_for_data_type(pair.second.layout.data_type);

                pair.second.layout.stride = total_attrib_size_in_bytes;
                pair.second.layout.offset = total_attrib_size_in_bytes * pair.second.layout.idx;
            }
        }

        // Index component
        //-------------------------------------------------------------------------
        index_component::index_component(u32* idxs, u64 count)
            : m_indices(idxs)
            , m_index_count(count)
        {}

        //-------------------------------------------------------------------------
        const u32* index_component::indices() const { return m_indices; }

        //-------------------------------------------------------------------------
        u64 index_component::index_count() const { return m_index_count; }

        // Texture Component
        //-------------------------------------------------------------------------
        texture_component::texture_component(u32 id) 
            : m_texture_id(id)
        {}

        u32 texture_component::texture_id() const { return m_texture_id; }

        //-------------------------------------------------------------------------
        std::unique_ptr<vertex_component> make_vertex_component(u64 count)
        {
            return std::make_unique<vertex_component>(count);
        }

        //-------------------------------------------------------------------------
        std::unique_ptr<index_component> make_index_component(u32* idxs, u64 count)
        {
            return std::make_unique<index_component>(idxs, count);
        }

        //-------------------------------------------------------------------------
        std::unique_ptr<texture_component> make_texture_component(u32 id)
        {
            return std::make_unique<texture_component>(id);
        }
    }
}