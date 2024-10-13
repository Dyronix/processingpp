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
            for (u64 i = 0; i < data_count; ++i)
            {
                m_buffer[m_current_index_count + i] = data_ptr[i];
            }

            m_current_index_count += data_count;
        }

        //-------------------------------------------------------------------------
        void index_buffer::free()
        {
            m_current_index_count = 0;
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
