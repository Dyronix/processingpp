#pragma once

#include "util/types.h"

#include <vector>
#include <functional>
#include <optional>

namespace ppp
{
    namespace memory
    {
        // Define a structure to represent each attribute layout
        struct tagged_buffer_layout
        {
            tagged_buffer_layout(u32 in_tag, u64 in_offset, u64 in_size);

            u32 tag;
            u64 offset;
            u64 size;
        };

        class tagged_buffer
        {
        public:
            tagged_buffer(const tagged_buffer_layout* layouts, u64 layout_count, u64 element_count);

            bool can_add_elements(u64 count) const;

            void set_element_data(u32 tag, const void* data_ptr, u64 data_count);
            void map_element_data(u32 tag, const void* data_ptr);

            void free();

            template<typename T>
            void transform_attribute_data(u32 tag, std::function<void(T&)> transform_func);
            template<typename T>
            void transform_attribute_data(u32 tag, u64 start_index, u64 end_index, std::function<void(T&)> transform_func);

            const std::vector<u8>& get_data() const;

            u64 element_size() const;
            u64 element_count() const;
            u64 active_element_count() const;

        private:
            const tagged_buffer_layout* find_layout(u32 tag) const;

            const tagged_buffer_layout* m_layouts; 
            const u64 m_layout_count;              

            u64 m_element_size;                    
            u64 m_element_count;                   
            u64 m_current_element_count;           

            std::vector<u8> m_buffer;              
        };

        template<typename T>
        void tagged_buffer::transform_attribute_data(u32 tag, std::function<void(T&)> transform_func)
        {
            transform_attribute_data<T>(tag, 0, m_current_element_count, transform_func);
        }

        template<typename T>
        void tagged_buffer::transform_attribute_data(u32 tag, u64 start_index, u64 end_index, std::function<void(T&)> transform_func)
        {
            const tagged_buffer_layout* attribute_layout = find_layout(tag);
            if (!attribute_layout)
            {
                log::error("Tag not found in layout!");
                return;
            }

            u64 attribute_offset = attribute_layout->offset;
            u64 attribute_stride = m_element_size;

            for (u64 i = start_index; i < end_index; ++i)
            {
                T* attribute_ptr = reinterpret_cast<T*>(m_buffer.data() + i * attribute_stride + attribute_offset);
                transform_func(*attribute_ptr);
            }
        }

    } // namespace render
} // namespace ppp
