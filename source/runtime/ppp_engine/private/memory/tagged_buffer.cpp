#include "memory/tagged_buffer.h"
#include "util/log.h"  // Adjust this include to where your logging system is located
#include <cstring>        // for memcpy

namespace ppp
{
    namespace memory
    {
        // TAGGED BUFFER LAYOUT
        //-------------------------------------------------------------------------
        tagged_buffer_layout::tagged_buffer_layout(u32 in_tag, u64 in_offset, u64 in_size)
            : tag(in_tag)
            , offset(in_offset)
            , size(in_size)
        {
        }

        // TAGGED BUFFER
        //-------------------------------------------------------------------------
        tagged_buffer::tagged_buffer(const tagged_buffer_layout* layouts, u64 layout_count, u64 element_count)
            : m_layouts(layouts)
            , m_layout_count(layout_count)
            , m_element_count(element_count)
            , m_current_element_count(0)
        {
            m_element_size = 0;

            for (u64 i = 0; i < layout_count; ++i)
            {
                m_element_size += layouts[i].size;
            }

            m_buffer.resize(m_element_size * element_count);
        }

        //-------------------------------------------------------------------------
        bool tagged_buffer::can_add_elements(u64 count) const
        {
            return (m_current_element_count + count <= m_element_count);
        }

        //-------------------------------------------------------------------------
        void tagged_buffer::set_element_data(u32 tag, const void* data_ptr, u64 data_count)
        {
            const tagged_buffer_layout* element_layout = find_layout(tag);
            if (!element_layout)
            {
                log::error("Tag not found in layout!");
                return;
            }

            u64 element_offset = element_layout->offset;
            u64 element_stride = m_element_size;
            u64 element_size = element_layout->size;

            u64 max_elements_to_set = std::min(data_count, m_element_count - m_current_element_count);

            for (u64 i = 0; i < max_elements_to_set; ++i)
            {
                const u8* src_ptr = reinterpret_cast<const u8*>(data_ptr) + i * element_size;
                u8* dst_ptr = m_buffer.data() + (m_current_element_count + i) * element_stride + element_offset;
                memcpy(dst_ptr, src_ptr, element_size);
            }

            m_current_element_count = std::max(m_current_element_count, m_current_element_count + max_elements_to_set);
        }

        //-------------------------------------------------------------------------
        void tagged_buffer::map_element_data(u32 tag, const void* data_ptr)
        {
            const tagged_buffer_layout* element_layout = find_layout(tag);
            if (!element_layout)
            {
                log::error("Tag not found in layout!");
                return;
            }

            u64 element_offset = element_layout->offset;
            u64 element_stride = m_element_size;
            u64 element_size = element_layout->size;

            for (u64 i = 0; i < m_current_element_count; ++i)
            {
                u8* dst_ptr = m_buffer.data() + i * element_stride + element_offset;
                memcpy(dst_ptr, data_ptr, element_size);
            }
        }

        //-------------------------------------------------------------------------
        void tagged_buffer::free()
        {
            m_current_element_count = 0;
        }

        //-------------------------------------------------------------------------
        const tagged_buffer_layout* tagged_buffer::find_layout(u32 tag) const
        {
            for (u64 i = 0; i < m_layout_count; ++i)
            {
                if (m_layouts[i].tag == tag)
                {
                    return &m_layouts[i];
                }
            }
            return nullptr;
        }

        //-------------------------------------------------------------------------
        const std::vector<u8>& tagged_buffer::get_data() const
        {
            return m_buffer;
        }

        //-------------------------------------------------------------------------
        u64 tagged_buffer::element_size() const
        {
            return m_element_size;
        }

        //-------------------------------------------------------------------------
        u64 tagged_buffer::element_count() const
        {
            return m_element_count;
        }

        //-------------------------------------------------------------------------
        u64 tagged_buffer::active_element_count() const
        {
            return m_current_element_count;
        }

    } // namespace render
} // namespace ppp
