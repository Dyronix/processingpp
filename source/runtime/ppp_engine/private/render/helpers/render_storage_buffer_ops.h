#pragma once

#include "render/render_storage_buffer.h"

namespace ppp
{
    namespace render
    {
        namespace storage_buffer_ops
        {
            struct storage_data_addition_scope
            {
                storage_data_addition_scope(storage_buffer& sb, u64 data_count);
                ~storage_data_addition_scope();

                // Disable copy and move to prevent misuse
                storage_data_addition_scope(const storage_data_addition_scope&) = delete;
                storage_data_addition_scope& operator=(const storage_data_addition_scope&) = delete;

                storage_data_addition_scope(storage_data_addition_scope&&) = delete;
                storage_data_addition_scope& operator=(storage_data_addition_scope&&) = delete;

                void                close();

                storage_buffer&     get_storage_buffer() const { return m_storage_buffer; }
                u64                 get_max_elemenst_to_set() const { return m_max_elements_to_set; }
                bool                is_active() const { return m_active; }

            private:
                storage_buffer&     m_storage_buffer;
                u64                 m_max_elements_to_set;
                bool                m_active;
            };

            void set_storage_data(storage_data_addition_scope& sdas, const void* data_ptr);
        }
    }
}