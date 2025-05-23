#include "render/helpers/render_instance_buffer_ops.h"

namespace ppp
{
    namespace render
    {
        namespace instance_buffer_ops
        {
            //-------------------------------------------------------------------------
            instance_addition_scope::instance_addition_scope(instance_buffer& vb, u64 data_count)
                : m_instance_buffer(vb)
                , m_active(true)
                , m_max_elements_to_set(data_count)
            {
                m_instance_buffer.open(data_count);
            }

            //-------------------------------------------------------------------------
            instance_addition_scope::~instance_addition_scope()
            {
                if (m_active)
                {
                    m_instance_buffer.close();
                }
            }

            //-------------------------------------------------------------------------
            void instance_addition_scope::close()
            {
                if (m_active)
                {
                    m_instance_buffer.close();
                    m_active = false;
                }
            }

            //-------------------------------------------------------------------------
            void set_instance_data(instance_addition_scope& ias, const void* data_ptr)
            {
                instance_buffer& ib = ias.get_instance_buffer();

                u64 size_layout = calculate_total_size_layout(ib.layouts(), ib.layout_count());

                memcpy(ib.data() + (ib.active_element_count() * size_layout), data_ptr, size_layout * ias.get_max_elemenst_to_set());
            }
        }
    }
}