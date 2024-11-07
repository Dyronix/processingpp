#pragma once

#include "render/render_item_components.h"

#include "util/log.h"

#include <vector>
#include <functional>

namespace ppp
{
    namespace render
    {
        class vertex_buffer
        {
        public:
            vertex_buffer(u64 vertex_count);
            vertex_buffer(const vertex_attribute_layout* layouts, u64 layout_count, u64 vertex_count);

            bool can_add_vertices(u64 count) const;
            bool has_layout(vertex_attribute_type type) const;

            void open_attribute_addition(u64 data_count);
            void close_attribute_addition();

            void set_attribute_data(vertex_attribute_type type, const void* data_ptr);
            void map_attribute_data(vertex_attribute_type type, const void* data_ptr);

            void free();

            template<typename T>
            void transform_attribute_data(vertex_attribute_type type,std::function<void(T&)> transform_func);
            template<typename T>
            void transform_attribute_data(vertex_attribute_type type,u64 start_index,u64 end_index,std::function<void(T&)> transform_func);

            std::vector<u8>& get_data();
            const std::vector<u8>& get_data() const;

            u64 size_in_bytes() const;
            u64 vertex_size() const;
            u64 vertex_count() const;
            u64 active_vertex_count() const;

        private:
            const vertex_attribute_layout* find_layout(vertex_attribute_type type) const;

            const vertex_attribute_layout* m_layouts;
            const u64 m_layout_count;

            u64 m_vertex_size;
            u64 m_vertex_count;
            u64 m_current_vertex_count;
            
            std::vector<u8> m_buffer;
        };

        template<typename T>
        void vertex_buffer::transform_attribute_data(vertex_attribute_type type, std::function<void(T&)> transform_func)
        {
            transform_attribute_data<T>(tag, 0, m_current_element_count, transform_func);
        }

        template<typename T>
        void vertex_buffer::transform_attribute_data(vertex_attribute_type type, u64 start_index, u64 end_index, std::function<void(T&)> transform_func)
        {
            const vertex_attribute_layout* attribute_layout = find_layout(type);
            if (!attribute_layout)
            {
                log::error("Tag not found in layout!");
                return;
            }

            u64 attribute_offset = attribute_layout->offset;
            u64 attribute_stride = m_vertex_size;

            for (u64 i = start_index; i < end_index; ++i)
            {
                T* attribute_ptr = reinterpret_cast<T*>(m_buffer.data() + i * attribute_stride + attribute_offset);
                transform_func(*attribute_ptr);
            }
        }
    }
}