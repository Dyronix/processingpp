#include "render/render_index_buffer.h"

#include "util/log.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        index_buffer::index_buffer(u64 index_count)
            : m_index_count(index_count)
            , m_current_index_count(0)
        {
            m_buffer = std::make_unique<u32[]>(m_index_count);
        }

        //-------------------------------------------------------------------------
        bool index_buffer::can_add_indices(u64 count) const
        {
            return m_current_index_count + count <= m_index_count;
        }

        //-------------------------------------------------------------------------
        void index_buffer::set_index_data(const u32* data_ptr, u64 data_count)
        {
            memcpy(m_buffer.get() + m_current_index_count, data_ptr, sizeof(u32) * data_count);

            m_current_index_count += data_count;
        }

        //-------------------------------------------------------------------------
        void index_buffer::free()
        {
            m_current_index_count = 0;
        }

        //-------------------------------------------------------------------------
        void index_buffer::transform_index_data(std::function<void(u32&)> transform_func)
        {
            transform_index_data(0, m_current_index_count, transform_func);
        }

        //-------------------------------------------------------------------------
        void index_buffer::transform_index_data(u64 start_index, u64 end_index, std::function<void(u32&)> transform_func)
        {
            for (u64 i = start_index; i < end_index; ++i)
            {
                u32* index_ptr = reinterpret_cast<u32*>(m_buffer.get() + i);
                transform_func(*index_ptr);
            }
        }

        //-------------------------------------------------------------------------
        u32* index_buffer::get_data()
        {
            return m_buffer.get();
        }

        //-------------------------------------------------------------------------
        const u32* index_buffer::get_data() const
        {
            return m_buffer.get();
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::size_in_bytes() const
        {
            return sizeof(m_buffer[0]) * m_current_index_count;
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::index_size() const
        {
            return sizeof(m_buffer[0]);
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::index_count() const
        {
            return m_index_count;
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::active_index_count() const
        {
            return m_current_index_count;
        }
    }
}
