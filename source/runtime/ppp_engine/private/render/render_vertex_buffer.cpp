#include "render/render_vertex_buffer.h"

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            struct vertex_buffer_meta
            {
                bool is_open;

                u64 max_elements_to_set;
            };

            std::unordered_map<vertex_buffer*, vertex_buffer_meta> _vb_addition;
        }

        //-------------------------------------------------------------------------
        vertex_buffer::vertex_buffer(const vertex_attribute_layout* layouts, u64 layout_count, u64 vertex_count)
            : m_layouts(layouts)
            , m_layout_count(layout_count)
            , m_vertex_count(vertex_count)
            , m_current_vertex_count(0)
        {
            m_vertex_size = 0;

            for (u64 i = 0; i < layout_count; ++i)
            {
                m_vertex_size += layouts[i].total_size_in_bytes();
            }

            m_buffer.resize(m_vertex_size * vertex_count);
        }

        //-------------------------------------------------------------------------
        bool vertex_buffer::can_add_vertices(u64 count) const
        {
            return (m_current_vertex_count + count <= m_vertex_count);
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::open_attribute_addition(u64 data_count)
        {
            internal::_vb_addition[this].is_open = true;
            internal::_vb_addition[this].max_elements_to_set = std::min(data_count, m_vertex_count - m_current_vertex_count);;
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::close_attribute_addition()
        {
            m_current_vertex_count = std::max(m_current_vertex_count, m_current_vertex_count + internal::_vb_addition[this].max_elements_to_set);

            internal::_vb_addition[this].max_elements_to_set = 0;
            internal::_vb_addition[this].is_open = false;
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::set_attribute_data(vertex_attribute_type type, const void* data_ptr)
        {
            assert(internal::_vb_addition[this].is_open);

            const vertex_attribute_layout* element_layout = find_layout(type);
            if (!element_layout)
            {
                log::error("Tag not found in layout!");
                return;
            }

            u64 element_offset = element_layout->offset;
            u64 element_stride = m_vertex_size;
            u64 element_size = element_layout->total_size_in_bytes();

            for (u64 i = 0; i < internal::_vb_addition[this].max_elements_to_set; ++i)
            {
                const u8* src_ptr = reinterpret_cast<const u8*>(data_ptr) + i * element_size;
                u8* dst_ptr = m_buffer.data() + (m_current_vertex_count + i) * element_stride + element_offset;
                memcpy(dst_ptr, src_ptr, element_size);
            }
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::map_attribute_data(vertex_attribute_type type, const void* data_ptr)
        {
            const vertex_attribute_layout* element_layout = find_layout(type);
            if (!element_layout)
            {
                log::error("Tag not found in layout!");
                return;
            }

            u64 element_offset = element_layout->offset;
            u64 element_stride = m_vertex_size;
            u64 element_size = element_layout->total_size_in_bytes();

            for (u64 i = 0; i < internal::_vb_addition[this].max_elements_to_set; ++i)
            {
                u8* dst_ptr = m_buffer.data() + (m_current_vertex_count + i) * element_stride + element_offset;
                memcpy(dst_ptr, data_ptr, element_size);
            }
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::free()
        {
            m_current_vertex_count = 0;
        }

        //-------------------------------------------------------------------------
        const vertex_attribute_layout* vertex_buffer::find_layout(vertex_attribute_type type) const
        {
            for (u64 i = 0; i < m_layout_count; ++i)
            {
                if (m_layouts[i].type == type)
                {
                    return &m_layouts[i];
                }
            }
            return nullptr;
        }

        //-------------------------------------------------------------------------
        std::vector<u8>& vertex_buffer::get_data()
        {
            return m_buffer;
        }

        //-------------------------------------------------------------------------
        const std::vector<u8>& vertex_buffer::get_data() const
        {
            return m_buffer;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::size_in_bytes() const
        {
            return m_vertex_size * m_current_vertex_count;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::vertex_size() const
        {
            return m_vertex_size;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::vertex_count() const
        {
            return m_vertex_count;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::active_vertex_count() const
        {
            return m_current_vertex_count;
        }

    } // namespace render
} // namespace ppp
