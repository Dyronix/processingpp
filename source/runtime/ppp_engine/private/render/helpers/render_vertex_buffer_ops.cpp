#include "render/helpers/render_vertex_buffer_ops.h"

namespace ppp
{
    namespace render
    {
        namespace vertex_buffer_ops
        {
            //-------------------------------------------------------------------------
            vertex_attribute_addition_scope::vertex_attribute_addition_scope(vertex_buffer& vb, u64 data_count)
                : m_vertex_buffer(vb)
                , m_active(true)
                , m_max_elements_to_set(data_count)
            {
                assert(data_count > m_vertex_buffer.vertex_count() - m_vertex_buffer.active_vertex_count() && "Vertex buffer overflow");

                m_vertex_buffer.open(data_count);
            }

            //-------------------------------------------------------------------------
            vertex_attribute_addition_scope::~vertex_attribute_addition_scope() 
            {
                if (m_active) 
                {
                    m_vertex_buffer.close();
                }
            }

            //-------------------------------------------------------------------------
            void vertex_attribute_addition_scope::close() 
            {
                if (m_active) 
                {
                    m_vertex_buffer.close();
                    m_active = false;
                }
            }

            //-------------------------------------------------------------------------
            void set_attribute_data(vertex_attribute_addition_scope& vaas, attribute_type type, const void* data_ptr)
            {
                vertex_buffer& vb = vaas.get_vertex_buffer();
                const attribute_layout* element_layout = vb.find_layout(type);
                if (!element_layout)
                {
                    log::error("Tag not found in layout!");
                    return;
                }

                u64 element_offset = element_layout->offset;
                u64 element_stride = vb.vertex_size_in_bytes();
                u64 element_size = element_layout->total_size_in_bytes();

                for (u64 i = 0; i < vaas.get_max_elemenst_to_set(); ++i)
                {
                    const u8* src_ptr = reinterpret_cast<const u8*>(data_ptr) + i * element_size;
                    u8* dst_ptr = vb.data() + (vb.active_vertex_count() + i) * element_stride + element_offset;
                    memcpy(dst_ptr, src_ptr, element_size);
                }
            }

            //-------------------------------------------------------------------------
            void map_attribute_data(vertex_attribute_addition_scope& vaas, attribute_type type, const void* data_ptr)
            {
                vertex_buffer& vb = vaas.get_vertex_buffer();
                const attribute_layout* element_layout = vb.find_layout(type);
                if (!element_layout)
                {
                    log::error("Tag not found in layout!");
                    return;
                }

                u64 element_offset = element_layout->offset;
                u64 element_stride = vb.vertex_size_in_bytes();
                u64 element_size = element_layout->total_size_in_bytes();

                for (u64 i = 0; i < vaas.get_max_elemenst_to_set(); ++i)
                {
                    u8* dst_ptr = vb.data() + (vb.active_vertex_count() + i) * element_stride + element_offset;
                    memcpy(dst_ptr, data_ptr, element_size);
                }
            }
        }
    }
}