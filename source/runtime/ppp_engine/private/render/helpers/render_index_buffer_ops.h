#pragma once

#include "render/render_index_buffer.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace render
    {
        namespace index_buffer_ops
        {
            struct index_addition_scope
            {
                index_addition_scope(index_buffer& vb, u64 data_count);
                ~index_addition_scope();

                // Disable copy and move to prevent misuse
                index_addition_scope(const index_addition_scope&) = delete;
                index_addition_scope& operator=(const index_addition_scope&) = delete;

                index_addition_scope(index_addition_scope&&) = delete;
                index_addition_scope& operator=(index_addition_scope&&) = delete;

                void                close();

                index_buffer&       get_index_buffer() const { return m_index_buffer; }
                u64                 get_max_elemenst_to_set() const { return m_max_elements_to_set; }
                bool                is_active() const { return m_active; }

            private:
                index_buffer&       m_index_buffer;
                u64                 m_max_elements_to_set;
                bool                m_active;
            };  

            void set_index_data(index_addition_scope& ias, const void* data_ptr);

            void transform_index_data(index_buffer& ib, std::function<void(index&)> transform_func);
            void transform_index_data(index_buffer& ib, u64 start_index, u64 end_index, std::function<void(index&)> transform_func);
        }
    }
}