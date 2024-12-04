#pragma once

#include "render/render_vertex_buffer.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace render
    {
        class vertex_buffer;

        namespace vertex_buffer_ops
        {
            struct vertex_attribute_data
            {
                vertex_attribute_type type;
                const void* data_ptr;
            };

            struct vertex_attribute_addition_scope
            {
                vertex_attribute_addition_scope(vertex_buffer& vb, u64 data_count);
                ~vertex_attribute_addition_scope();

                // Disable copy and move to prevent misuse
                vertex_attribute_addition_scope(const vertex_attribute_addition_scope&) = delete;
                vertex_attribute_addition_scope& operator=(const vertex_attribute_addition_scope&) = delete;

                void close();

            private:
                vertex_buffer& m_vertex_buffer;
                bool m_active;
            };

            void set_attribute_data(vertex_attribute_addition_scope& vaas, const std::vector<vertex_attribute_data>& attribute_data);

            void map_attribute_data(vertex_attribute_addition_scope& vaas, const std::vector<vertex_attribute_data>& attribute_data);

            template<typename T>
            void transform_attribute_data(vertex_buffer& vb, vertex_attribute_type type, std::function<void(T&)> transform_func)
            {
                transform_attribute_data<T>(type, 0, vb.active_vertex_count(), transform_func);
            }

            template<typename T>
            void transform_attribute_data(vertex_attribute_type type,u64 start_index,u64 end_index,std::function<void(T&)> transform_func)
            {
                const vertex_attribute_layout* attribute_layout = vb.find_layout(type);
                if (!attribute_layout)
                {
                    log::error("Tag not found in layout!");
                    return;
                }

                u64 attribute_offset = attribute_layout->offset;
                u64 attribute_stride = m_vertex_size;

                for (u64 i = start_index; i < end_index; ++i)
                {
                    T* attribute_ptr = reinterpret_cast<T*>(vb.data() + i * attribute_stride + attribute_offset);
                    transform_func(*attribute_ptr);
                }
            }
        }
    }
}