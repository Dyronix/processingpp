#pragma once

#include "render/render_item_components.h"

#include <vector>
#include <functional>

namespace ppp
{
    namespace render
    {
        class vertex_buffer
        {
        public:
            vertex_buffer(const vertex_attribute_layout* layouts, size_t layout_count, size_t vertex_count)
                : m_layouts(layouts), m_layout_count(layout_count), m_vertex_count(vertex_count) {
                // Calculate the total size of the buffer
                m_vertex_size = 0;
                for (int i = 0; i < layout_count; ++i) {
                    m_vertex_size += layouts[i].total_size_in_bytes();
                }
                m_buffer.resize(m_vertex_size * vertex_count);
            }

            // Method to check if we can add 'x' vertices for a specific attribute type
            bool can_add_vertices(size_t count) const {
                return (m_current_vertex_count + count <= m_vertex_count);
            }

            // Set attribute data for all vertices (bulk, array of values)
            void set_attribute_data(vertex_attribute_type type, const void* data_ptr, size_t data_count)
            {
                const vertex_attribute_layout* attribute_layout = find_layout(type);
                if (!attribute_layout) {
                    std::cerr << "Attribute type not found in layout!" << std::endl;
                    return;
                }

                size_t attribute_offset = attribute_layout->offset;
                size_t attribute_stride = m_vertex_size;
                size_t element_size = attribute_layout->total_size_in_bytes();

                // Determine the number of vertices we can set
                size_t max_elements_to_set = std::min(data_count, m_vertex_count - m_current_vertex_count);

                // Copy data into the buffer
                for (size_t i = 0; i < max_elements_to_set; ++i) {
                    const uint8_t* src_ptr = reinterpret_cast<const uint8_t*>(data_ptr) + i * element_size;
                    uint8_t* dst_ptr = m_buffer.data() + (m_current_vertex_count + i) * attribute_stride + attribute_offset;
                    memcpy(dst_ptr, src_ptr, element_size);
                }

                // Update the current vertex count to be the maximum set so far
                m_current_vertex_count = std::max(m_current_vertex_count, m_current_vertex_count + max_elements_to_set);
            }

            // Map a single attribute value to all previously set vertices
            void map_attribute_data(vertex_attribute_type type, const void* data_ptr)
            {
                const vertex_attribute_layout* attribute_layout = find_layout(type);
                if (!attribute_layout) {
                    std::cerr << "Attribute type not found in layout!" << std::endl;
                    return;
                }

                size_t attribute_offset = attribute_layout->offset;
                size_t attribute_stride = m_vertex_size;
                size_t element_size = attribute_layout->total_size_in_bytes();

                // Apply the provided data to all previously set vertices
                for (size_t i = 0; i < m_current_vertex_count; ++i) {
                    uint8_t* dst_ptr = m_buffer.data() + i * attribute_stride + attribute_offset;
                    memcpy(dst_ptr, data_ptr, element_size);
                }
            }

            // Transform attribute data by applying a function to each element (e.g., transform positions)
            template<typename T>
            void transform_attribute_data(vertex_attribute_type type, std::function<void(T&)> transform_func) 
            {
                transform_attribute_data<T>(type, 0, m_current_vertex_count, transform_func);
            }

            // Transform attribute data by applying a function to each element (e.g., transform positions)
            template<typename T>
            void transform_attribute_data(vertex_attribute_type type, size_t start_index, size_t end_index, std::function<void(T&)> transform_func) {
                const vertex_attribute_layout* attribute_layout = find_layout(type);
                if (!attribute_layout) {
                    std::cerr << "Attribute type not found in layout!" << std::endl;
                    return;
                }

                size_t attribute_offset = attribute_layout->offset;
                size_t attribute_stride = m_vertex_size;

                // Transform only the specified range of vertices
                for (size_t i = start_index; i < end_index; ++i) {
                    T* attribute_ptr = reinterpret_cast<T*>(m_buffer.data() + i * attribute_stride + attribute_offset);
                    transform_func(*attribute_ptr);
                }
            }

            // Get the raw buffer data
            const std::vector<uint8_t>& get_data() const {
                return m_buffer;
            }

            void reset()
            {
                m_current_vertex_count = 0;
            }

            s32 active_vertex_count() const { return m_current_vertex_count; }

        private:
            const vertex_attribute_layout* find_layout(vertex_attribute_type type) const {
                for (size_t i = 0; i < m_layout_count; ++i) {
                    if (m_layouts[i].type == type) {
                        return &m_layouts[i];
                    }
                }
                return nullptr;
            }

            const vertex_attribute_layout* m_layouts; // Pointer to an array of layouts
            size_t m_layout_count; // Number of layouts
            size_t m_vertex_size;  // Total size of one vertex
            size_t m_vertex_count; // Total number of vertices
            size_t m_current_vertex_count = 0; // Number of vertices currently set
            std::vector<uint8_t> m_buffer; // Buffer storing vertex data
        };
    }
}