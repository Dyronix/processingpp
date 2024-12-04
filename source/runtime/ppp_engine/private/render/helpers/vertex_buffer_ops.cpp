#include "render/helpers/vertex_buffer_ops.h"

namespace ppp
{
    namespace render
    {
        namespace vertex_buffer_ops
        {
            namespace internal
            {
                struct vertex_buffer_meta
                {
                    bool is_open;

                    u64 max_elements_to_set;
                };

                std::unordered_map<vertex_buffer*, vertex_buffer_meta> _vb_addition;

                //-------------------------------------------------------------------------
                void open_attribute_addition(vertex_buffer& vb, u64 data_count)
                {
                    internal::_vb_addition[&vb].is_open = true;
                    internal::_vb_addition[&vb].max_elements_to_set = std::min(data_count, vb.vertex_count() - vb.active_vertex_count());
                }

                //-------------------------------------------------------------------------
                void close_attribute_addition(vertex_buffer& vb)
                {
                    m_current_vertex_count = std::max(vb.active_vertex_count(), vb.active_vertex_count() + internal::_vb_addition[&vb].max_elements_to_set);

                    internal::_vb_addition[&vb].max_elements_to_set = 0;
                    internal::_vb_addition[&vb].is_open = false;
                }
            }

            vertex_attribute_addition_scope::vertex_attribute_addition_scope(vertex_buffer& vb, u64 data_count)
                : m_vertex_buffer(vb)
                , m_active(true) 
            {
                m_vertex_buffer.open_attribute_addition(data_count);
            }

            vertex_attribute_addition_scope::~vertex_attribute_addition_scope() 
            {
                if (m_active) 
                {
                    m_vertex_buffer.close_attribute_addition();
                }
            }

            void vertex_attribute_addition_scope::close() 
            {
                if (m_active) 
                {
                    m_vertex_buffer.close_attribute_addition();
                    m_active = false;
                }
            }

            //-------------------------------------------------------------------------
            void set_attribute_data(vertex_buffer& vb, u64 vertex_count_to_add, const std::vector<vertex_attribute_data>& attribute_data)
            {
                u64 max_elements = std::min(vertex_count_to_add, vb.vertex_count() - vb.active_vertex_count());

                for(auto& data : attribute_data)
                {
                    const vertex_attribute_layout* element_layout = vb.find_layout(data.type);
                    if (!element_layout)
                    {
                        log::error("Tag not found in layout!");
                        return;
                    }

                    u64 element_offset = element_layout->offset;
                    u64 element_stride = vb.vertex_size_in_bytes();
                    u64 element_size = element_layout->total_size_in_bytes();

                    for (u64 i = 0; i < max_elements; ++i)
                    {
                        const u8* src_ptr = reinterpret_cast<const u8*>(data.data_ptr) + i * element_size;
                        u8* dst_ptr = vb.data() + (vb.active_vertex_count() + i) * element_stride + element_offset;
                        memcpy(dst_ptr, src_ptr, element_size);
                    }
                }
            }

            //-------------------------------------------------------------------------
            void map_attribute_data(vertex_buffer& vb, u64 vertex_count_to_set, const std::vector<vertex_attribute_data>& attribute_data)
            {
                for(auto& data : attribute_data)
                {
                    const vertex_attribute_layout* element_layout = vb.find_layout(data.type);
                    if (!element_layout)
                    {
                        log::error("Tag not found in layout!");
                        return;
                    }

                    u64 element_offset = element_layout->offset;
                    u64 element_stride = vb.vertex_size_in_bytes();
                    u64 element_size = element_layout->total_size_in_bytes();

                    for (u64 i = 0; i < internal::_vb_addition[this].max_elements_to_set; ++i)
                    {
                        u8* dst_ptr = vb.data() + (vb.active_vertex_count() + i) * element_stride + element_offset;
                        memcpy(dst_ptr, data.data_ptr, element_size);
                    }
                }
            }
        }
    }
}