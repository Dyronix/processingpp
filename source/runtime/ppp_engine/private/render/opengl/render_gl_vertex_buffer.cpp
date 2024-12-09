#include "render/render_vertex_buffer.h"

#include "render/opengl/render_gl_error.h"

#include <glad/glad.h>

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
                }
                return 0;  // Fallback to avoid compiler warnings
            }
        }

        struct vertex_buffer::impl
        {
            //-------------------------------------------------------------------------
            impl(const attribute_layout* layouts, u64 layout_count, u64 vertex_count)
                : layouts(layouts)
                , layout_count(layout_count)
                , vertex_count(vertex_count)
                , current_vertex_count(0)
                , max_elements_to_set(0)
                , buffer()
                , vbo(0)
                , is_bound(false)
            {
                auto vertex_size = calculate_total_size_layout(layouts, layout_count);

                buffer.resize(vertex_size * vertex_count);

                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertex_size * vertex_count, nullptr, GL_DYNAMIC_DRAW);

                for (u64 i = 0; i < layout_count; ++i)
                {
                    const attribute_layout& layout = layouts[i];

                    for (s32 j = 0; j < layout.span; ++j)
                    {
                        GL_CALL(glEnableVertexAttribArray(i + j));
                        GL_CALL(glVertexAttribPointer(i + j, layout.count, internal::convert_to_gl_data_type(layout.data_type), layout.normalized ? GL_TRUE : GL_FALSE, layout.stride, (void*)layout.offset));
                    }
                }
            }

            //-------------------------------------------------------------------------
            ~impl()
            {
                if (vbo)
                {
                    glDeleteBuffers(1, &vbo);
                }
            }

            //-------------------------------------------------------------------------
            void bind() const
            {
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                is_bound = true;
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                is_bound = false;
            }

            //-------------------------------------------------------------------------
            void free()
            {
                glDeleteBuffers(1, &vbo);
                vbo = 0;
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                assert(is_bound && "Cannot upload data to an unbound buffer");

                const u64 vertex_buffer_byte_size =  calculate_total_size_layout(layouts, layout_count);

                GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_byte_size * vertex_count, buffer.data()));
            }

            //-------------------------------------------------------------------------
            const attribute_layout*         layouts;
            const u64                       layout_count;

            u64                             vertex_count;
            u64                             current_vertex_count;
            u64                             max_elements_to_set;
            
            std::vector<u8>                 buffer;

            u32                             vbo;

            mutable bool                    is_bound;
        };

        //-------------------------------------------------------------------------
        vertex_buffer::vertex_buffer(const attribute_layout* layouts, u64 layout_count, u64 vertex_count)
            : m_pimpl(std::make_unique<impl>(layouts, layout_count, vertex_count))
        {

        }

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
