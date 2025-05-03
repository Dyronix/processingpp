#pragma once

#include "render/render_vertex_buffer.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace render
    {
        namespace vertex_buffer_ops
        {
            struct vertex_attribute_addition_scope
            {
                vertex_attribute_addition_scope(vertex_buffer& vb, u64 data_count);
                ~vertex_attribute_addition_scope();

                // Disable copy and move to prevent misuse
                vertex_attribute_addition_scope(const vertex_attribute_addition_scope&) = delete;
                vertex_attribute_addition_scope& operator=(const vertex_attribute_addition_scope&) = delete;

                vertex_attribute_addition_scope(vertex_attribute_addition_scope&&) = delete;
                vertex_attribute_addition_scope& operator=(vertex_attribute_addition_scope&&) = delete;

                void                close();

                vertex_buffer&      get_vertex_buffer() const { return m_vertex_buffer; }
                u64                 get_max_elemenst_to_set() const { return m_max_elements_to_set; }
                bool                is_active() const { return m_active; }

            private:
                vertex_buffer&      m_vertex_buffer;
                u64                 m_max_elements_to_set;
                bool                m_active;
            };  

            //-------------------------------------------------------------------------
            void set_attribute_data(vertex_attribute_addition_scope& vaas, attribute_type type, const void* data_ptr);

            //-------------------------------------------------------------------------
            void map_attribute_data(vertex_attribute_addition_scope& vaas, attribute_type type, const void* data_ptr);

            //-------------------------------------------------------------------------
            template<typename T>
            void transform_attribute_data(vertex_buffer& vb, attribute_type type, std::function<void(T&)> transform_func)
            {
                transform_attribute_data<T>(vb, type, 0, vb.active_vertex_count(), transform_func);
            }

            //-------------------------------------------------------------------------
            template<typename T>
            void transform_attribute_data(vertex_buffer& vb, attribute_type type, u64 start_index,u64 end_index,std::function<void(T&)> transform_func)
            {
                const attribute_layout* attribute_layout = vb.find_layout(type);
                if (!attribute_layout)
                {
                    log::error("Attribute type not found in layout: {}", conversions::to_string(type));
                    return;
                }

                u64 attribute_offset = attribute_layout->offset;
                u64 attribute_stride = vb.element_size_in_bytes();

                for (u64 i = start_index; i < end_index; ++i)
                {
                    T* attribute_ptr = reinterpret_cast<T*>(vb.data() + i * attribute_stride + attribute_offset);
                    transform_func(*attribute_ptr);
                }
            }
        }
    }
}