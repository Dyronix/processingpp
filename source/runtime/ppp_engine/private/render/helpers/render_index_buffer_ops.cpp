#include "render/helpers/render_index_buffer_ops.h"

namespace ppp
{
    namespace render
    {
        namespace index_buffer_ops
        {
            //-------------------------------------------------------------------------
            index_addition_scope::index_addition_scope(index_buffer& vb, u64 data_count)
                : m_index_buffer(vb)
                , m_active(true)
                , m_max_elements_to_set(data_count)
            {
                assert(data_count < m_index_buffer.index_count() - m_index_buffer.active_index_count() && "index buffer overflow");

                m_index_buffer.open(data_count);
            }

            //-------------------------------------------------------------------------
            index_addition_scope::~index_addition_scope() 
            {
                if (m_active) 
                {
                    m_index_buffer.close();
                }
            }

            //-------------------------------------------------------------------------
            void index_addition_scope::close() 
            {
                if (m_active) 
                {
                    m_index_buffer.close();
                    m_active = false;
                }
            }

            //-------------------------------------------------------------------------
            void set_index_data(index_addition_scope& ias, const void* data_ptr)
            {
                index_buffer& ib = ias.get_index_buffer();

                memcpy(ib.data() + (ib.active_index_count() * sizeof(index)), data_ptr, sizeof(index) * ias.get_max_elemenst_to_set());
            }

            //-------------------------------------------------------------------------
            void transform_index_data(index_buffer& ib, std::function<void(index&)> transform_func)
            {
                transform_index_data(ib, 0, ib.active_index_count(), transform_func);
            }

            //-------------------------------------------------------------------------
            void transform_index_data(index_buffer& ib, u64 start_index, u64 end_index, std::function<void(index&)> transform_func)
            {
                for (u64 i = start_index; i < end_index; ++i)
                {
                    index* index_ptr = reinterpret_cast<index*>(ib.data() + i);
                    transform_func(*index_ptr);
                }
            }
        }
    }
}