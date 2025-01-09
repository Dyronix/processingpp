#include "render/render_storage_buffer.h"

#include "render/opengl/render_gl_error.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        struct storage_buffer::impl
        {
            //------------------------------------------------------------------------
            impl(u64 in_element_count, u64 in_element_size)
                : element_size(in_element_size)
                , element_count(in_element_count)
                , current_element_count(0)
                , max_elements_to_set(0)
                , buffer()
                , ssbo(0)
            {
                buffer.resize(in_element_size * in_element_count);

                GL_CALL(glGenBuffers(1, &ssbo));
                GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo));
                GL_CALL(glBufferData(GL_SHADER_STORAGE_BUFFER, in_element_size * in_element_count, nullptr, GL_DYNAMIC_DRAW));

                GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
            }

            //------------------------------------------------------------------------
            ~impl()
            {
                if (ssbo)
                {
                    GL_CALL(glDeleteBuffers(1, &ssbo));
                }
            }

            //------------------------------------------------------------------------
            void bind(u32 binding_point) const
            {
                GL_CALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo));
            }

            //------------------------------------------------------------------------
            void unbind() const
            {
                GL_CALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0));
            }

            //------------------------------------------------------------------------
            void free()
            {
                GL_CALL(glDeleteBuffers(1, &ssbo));
                ssbo = 0;
            }

            //------------------------------------------------------------------------
            void submit(u32 binding_point) const
            {
                const u64 element_size = buffer.size() / element_count;

                bind(binding_point);

                GL_CALL(glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, element_size * element_count, buffer.data()));
            }

            //------------------------------------------------------------------------
            u64                             element_size;
            u64                             element_count;
            u64                             current_element_count;
            u64                             max_elements_to_set;

            std::vector<u8>                 buffer;

            u32                             ssbo;
        };

        //------------------------------------------------------------------------
        storage_buffer::storage_buffer(u64 element_count, u64 element_size)
            : m_pimpl(std::make_unique<impl>(element_count, element_size))
        {

        }

        //------------------------------------------------------------------------
        storage_buffer::~storage_buffer() = default;

        //------------------------------------------------------------------------
        void storage_buffer::bind(u32 binding_point) const
        {
            m_pimpl->bind(binding_point);
        }

        //------------------------------------------------------------------------
        void storage_buffer::unbind() const
        {
            m_pimpl->unbind();
        }

        //------------------------------------------------------------------------
        void storage_buffer::submit(u32 binding_point) const
        {
            m_pimpl->submit(binding_point);
        }

        //-------------------------------------------------------------------------
        void storage_buffer::open(u64 max_elements_to_set)
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }

        //-------------------------------------------------------------------------
        void storage_buffer::close()
        {
            m_pimpl->current_element_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;
        }

        //------------------------------------------------------------------------
        void storage_buffer::reset()
        {
            m_pimpl->current_element_count = 0;
        }

        //------------------------------------------------------------------------
        void storage_buffer::free()
        {
            m_pimpl->free();

            m_pimpl->buffer.clear();

            m_pimpl->unbind();
            m_pimpl->free();
        }

        //------------------------------------------------------------------------
        u8* storage_buffer::data()
        {
            return m_pimpl->buffer.data();
        }

        //------------------------------------------------------------------------
        const u8* storage_buffer::data() const
        {
            return m_pimpl->buffer.data();
        }

        //------------------------------------------------------------------------
        u64 storage_buffer::total_size_in_bytes() const
        {
            return m_pimpl->current_element_count * m_pimpl->element_size;
        }

        //------------------------------------------------------------------------
        u64 storage_buffer::element_size_in_bytes() const
        {
            return m_pimpl->element_size;
        }

        //------------------------------------------------------------------------
        u64 storage_buffer::element_count() const
        {
            return m_pimpl->element_count;
        }

        //------------------------------------------------------------------------
        u64 storage_buffer::active_element_count() const
        {
            return m_pimpl->current_element_count;
        }

    } // namespace render
} // namespace ppp
