#include "render/render_index_buffer.h"

#include "render/opengl/render_gl_error.h"

#include <glad/glad.h>

#include "util/log.h"

namespace ppp
{
    namespace render
    {
        class index_buffer::impl
        {
        public:
            //-------------------------------------------------------------------------
            impl(u64 count)
                : index_count(count)
                , current_index_count(0)
                , max_elements_to_set(0)
                , buffer()
                , ebo(0)
            {
                const size_t size_ebo = sizeof(index) * count;

                buffer.resize(size_ebo);

                GL_CALL(glGenBuffers(1, &ebo));
                GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
                GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW));
                GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
            }

            //-------------------------------------------------------------------------
            ~impl()
            {
                if (ebo)
                {
                    GL_CALL(glDeleteBuffers(1, &ebo));
                }
            }

            //-------------------------------------------------------------------------
            void bind() const
            {
                GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                const u64 index_buffer_byte_size = sizeof(index) * index_count;

                bind();
                GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index_buffer_byte_size, buffer.data()));
            }

            //-------------------------------------------------------------------------
            void free()
            {
                GL_CALL(glDeleteBuffers(1, &ebo));
                ebo = 0;
            }

            u64                             index_count;
            u64                             current_index_count;
            u64                             max_elements_to_set;

            std::vector<u8>                 buffer;

            u32                             ebo;
        };

        //-------------------------------------------------------------------------
        index_buffer::index_buffer(u64 index_count)
            : m_pimpl(std::make_unique<impl>(index_count))
        {}

        //-------------------------------------------------------------------------
        index_buffer::~index_buffer() = default;

        //-------------------------------------------------------------------------
        void index_buffer::bind() const
        {
            m_pimpl->bind();
        }

        //-------------------------------------------------------------------------
        void index_buffer::unbind() const
        {
            m_pimpl->unbind();            
        }

        //-------------------------------------------------------------------------
        void index_buffer::submit() const
        {
            m_pimpl->submit();
        }
    
        //-------------------------------------------------------------------------
        void index_buffer::open(u64 max_elements_to_set)
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }
        
        //-------------------------------------------------------------------------
        void index_buffer::close()
        {
            m_pimpl->current_index_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;
        }

        //-------------------------------------------------------------------------
        void index_buffer::reset()
        {
            m_pimpl->current_index_count = 0;
        }

        //-------------------------------------------------------------------------
        void index_buffer::free()
        {
            reset();

            m_pimpl->buffer.clear();

            m_pimpl->unbind();
            m_pimpl->free();
        }

        //-------------------------------------------------------------------------
        u8* index_buffer::data()
        {
            return m_pimpl->buffer.data();
        }

        //-------------------------------------------------------------------------
        const u8* index_buffer::data() const
        {
            return m_pimpl->buffer.data();
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::total_size_in_bytes() const
        {
            return sizeof(m_pimpl->buffer[0]) * m_pimpl->current_index_count;
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::index_size_in_bytes() const
        {
            return sizeof(m_pimpl->buffer[0]);
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::index_count() const
        {
            return m_pimpl->index_count;
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::active_index_count() const
        {
            return m_pimpl->current_index_count;
        }
    }
}
