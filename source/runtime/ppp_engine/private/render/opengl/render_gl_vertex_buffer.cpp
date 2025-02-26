#include "render/render_vertex_buffer.h"

#include "render/opengl/render_gl_error.h"

#include "memory/memory_unique_ptr_util.h"

#include <glad/glad.h>

#include <assert.h>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static GLenum convert_to_gl_data_type(attribute_data_type type)
            {
                switch (type)
                {
                case attribute_data_type::FLOAT: return GL_FLOAT;
                case attribute_data_type::UNSIGNED_INT: return GL_UNSIGNED_INT;
                case attribute_data_type::INT: return GL_INT;
                }

                assert(false);
                return 0;  // Fallback to avoid compiler warnings
            }
        }

        struct vertex_buffer::impl
        {
            //-------------------------------------------------------------------------
            impl(u64 vertex_count, const attribute_layout* layouts, u64 layout_count, u64 layout_id_offset)
                : layouts(layouts)
                , layout_count(layout_count)
                , vertex_count(vertex_count)
                , previous_vertex_count(0)
                , current_vertex_count(0)
                , max_elements_to_set(0)
                , buffer()
                , vbo(0)
            {
                auto vertex_size = calculate_total_size_layout(layouts, layout_count);

                buffer.reserve(vertex_size * vertex_count);
                buffer.resize(vertex_size * vertex_count);

                GL_CALL(glGenBuffers(1, &vbo));
                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
                GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertex_size * vertex_count, nullptr, GL_DYNAMIC_DRAW));

                u64 attribute_index_offset = layout_id_offset;
                u64 attribute_stride_offset = 0;

                u64 attribute_index = 0;
                u64 attribute_offset = 0;

                for (u64 i = 0; i < layout_count; ++i)
                {
                    const attribute_layout& layout = layouts[i];

                    for (s32 j = 0; j < layout.span; ++j)
                    {
                        attribute_index = attribute_index_offset + i + j;
                        attribute_offset = attribute_stride_offset + j * layout.count * layout.element_size_in_bytes();

                        GL_CALL(glEnableVertexAttribArray(attribute_index));
                        switch (layout.data_type)
                        {
                        case attribute_data_type::FLOAT:
                            GL_CALL(glVertexAttribPointer(attribute_index, layout.count, internal::convert_to_gl_data_type(layout.data_type), layout.normalized ? GL_TRUE : GL_FALSE, layout.stride, (void*)attribute_offset));
                            break;
                        case attribute_data_type::UNSIGNED_INT: // fallthrough
                        case attribute_data_type::INT:
                            GL_CALL(glVertexAttribIPointer(attribute_index, layout.count, internal::convert_to_gl_data_type(layout.data_type), layout.stride, (void*)attribute_offset));
                            break;
                        }
                    }

                    attribute_index_offset = layout_id_offset + (layout.span - 1);
                    attribute_stride_offset += layout.total_size_in_bytes();
                }
                
                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
            }

            //-------------------------------------------------------------------------
            ~impl()
            {
                if (vbo)
                {
                    GL_CALL(glDeleteBuffers(1, &vbo));
                }
            }

            //-------------------------------------------------------------------------
            void bind() const
            {
                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
            }

            //-------------------------------------------------------------------------
            void free()
            {
                GL_CALL(glDeleteBuffers(1, &vbo));
                vbo = 0;
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                const u64 vertex_buffer_byte_size = calculate_total_size_layout(layouts, layout_count);

                // Ensure that current_vertex_count hasn't decreased unexpectedly.
                assert(current_vertex_count >= previous_vertex_count && "Current vertex count decreased unexpectedly.");

                bind();

                if (current_vertex_count == previous_vertex_count)
                {
                    // No new vertices have been added, skip upload
                    return;
                }

                u64 buffer_offset = previous_vertex_count * vertex_buffer_byte_size;
                u64 buffer_size = (current_vertex_count - previous_vertex_count) * vertex_buffer_byte_size;

                GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, buffer_size, buffer.data()));
            }

            //-------------------------------------------------------------------------
            u64                             previous_vertex_count;
            
            const attribute_layout*         layouts;
            const u64                       layout_count;

            u64                             vertex_count;
            u64                             current_vertex_count;
            u64                             max_elements_to_set;
            
            stage_vector<u8>                buffer;

            u32                             vbo;
        };

        //-------------------------------------------------------------------------
        vertex_buffer::vertex_buffer(u64 vertex_count, const attribute_layout* layouts, u64 layout_count, u64 layout_id_offset)
            : m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(vertex_count, layouts, layout_count, layout_id_offset))
        {

        }

        //-------------------------------------------------------------------------
        vertex_buffer::~vertex_buffer() = default;

        //-------------------------------------------------------------------------
        void vertex_buffer::bind() const
        {
            m_pimpl->bind();
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::unbind() const
        {
            m_pimpl->unbind();            
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::submit() const
        {
            m_pimpl->submit();

            m_pimpl->previous_vertex_count = m_pimpl->current_vertex_count;
        }

        //-------------------------------------------------------------------------
        bool vertex_buffer::can_add(u64 max_elements_to_set) const
        {
            using memory_policy = typename decltype(m_pimpl->buffer)::allocator_type::memory_policy;

            auto heap = memory_policy::get_heap();
            auto available_memory = heap->block_total_size();

            u64 new_vertex_count = active_vertex_count() + max_elements_to_set;
            u64 vertices_size = calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count) * new_vertex_count;

            // Make sure we do not exceed the memory size of one block
            // Make sure we do not exceed the amount of vertices we can store
            return vertices_size < available_memory && new_vertex_count < m_pimpl->vertex_count;
        }
    
        //-------------------------------------------------------------------------
        void vertex_buffer::open(u64 max_elements_to_set)
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }
        
        //-------------------------------------------------------------------------
        void vertex_buffer::close()
        {
            m_pimpl->current_vertex_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::reset()
        {
            m_pimpl->previous_vertex_count = 0;
            m_pimpl->current_vertex_count = 0;
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::free()
        {
            reset();

            m_pimpl->buffer.clear();

            m_pimpl->unbind();
            m_pimpl->free();
        }

        //-------------------------------------------------------------------------
        const attribute_layout* vertex_buffer::find_layout(attribute_type type) const
        {
            for (u64 i = 0; i < m_pimpl->layout_count; ++i)
            {
                if (m_pimpl->layouts[i].type == type)
                {
                    return &m_pimpl->layouts[i];
                }
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        bool vertex_buffer::has_layout(attribute_type type) const
        {
            return find_layout(type) != nullptr;
        }

        //------------------------------------------------------------------------- 
        const attribute_layout* vertex_buffer::layouts() const
        {
            return m_pimpl->layouts;
        }
        
        //-------------------------------------------------------------------------
        const u64 vertex_buffer::layout_count() const
        {
            return m_pimpl->layout_count;
        }

        //-------------------------------------------------------------------------
        u8* vertex_buffer::data()
        {
            return m_pimpl->buffer.data();
        }

        //-------------------------------------------------------------------------
        const u8* vertex_buffer::data() const
        {
            return m_pimpl->buffer.data();
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::total_size_in_bytes() const
        {
            auto vertex_size = calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
            return vertex_size * m_pimpl->current_vertex_count;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::vertex_size_in_bytes() const
        {
            return calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::vertex_count() const
        {
            return m_pimpl->vertex_count;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::active_vertex_count() const
        {
            return m_pimpl->current_vertex_count;
        }

    } // namespace render
} // namespace ppp
