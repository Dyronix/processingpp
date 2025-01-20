#include "render/helpers/render_storage_buffer_ops.h"

#include "util/pointer_math.h"

namespace ppp
{
    namespace render
    {
        namespace storage_buffer_ops
        {
            //-------------------------------------------------------------------------
            storage_data_addition_scope::storage_data_addition_scope(storage_buffer& sb, u64 data_count)
                :m_storage_buffer(sb)
                ,m_active(true)
                ,m_max_elements_to_set(data_count)
            {
                m_storage_buffer.open(data_count);
            }

            //-------------------------------------------------------------------------
            storage_data_addition_scope::~storage_data_addition_scope()
            {
                if (m_active)
                {
                    m_storage_buffer.close();
                }
            }

            //-------------------------------------------------------------------------
            void storage_data_addition_scope::close()
            {
                if (m_active)
                {
                    m_storage_buffer.close();
                    m_active = false;
                }
            }

            //-------------------------------------------------------------------------
            void set_storage_data(storage_data_addition_scope& sdas, const void* data_ptr)
            {
                storage_buffer& sb = sdas.get_storage_buffer();

                u64 element_size = sb.element_size_in_bytes();

                memcpy(sb.data() + (sb.active_element_count() * element_size), data_ptr, element_size * sdas.get_max_elemenst_to_set());
            }
        }
    }
}