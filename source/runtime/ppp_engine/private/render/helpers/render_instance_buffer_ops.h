#pragma once

#include "render/render_instance_buffer.h"

namespace ppp
{
    namespace render
    {
        namespace instance_buffer_ops
        {
            struct instance_addition_scope
            {
                instance_addition_scope(instance_buffer& vb, u64 data_count);
                ~instance_addition_scope();

                // Disable copy and move to prevent misuse
                instance_addition_scope(const instance_addition_scope&) = delete;
                instance_addition_scope& operator=(const instance_addition_scope&) = delete;

                instance_addition_scope(instance_addition_scope&&) = delete;
                instance_addition_scope& operator=(instance_addition_scope&&) = delete;

                void                close();

                instance_buffer&    get_instance_buffer() const { return m_instance_buffer; }
                u64                 get_max_elemenst_to_set() const { return m_max_elements_to_set; }
                bool                is_active() const { return m_active; }

            private:
                instance_buffer&    m_instance_buffer;
                u64                 m_max_elements_to_set;
                bool                m_active;
            };

            void set_instance_data(instance_addition_scope& ias, const void* data_ptr);
        }
    }
}