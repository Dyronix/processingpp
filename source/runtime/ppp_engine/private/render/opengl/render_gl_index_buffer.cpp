#include "render/render_index_buffer.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

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
            explicit impl(const u32 count)
                : previous_index_count(0)
                , index_count(count)
                , current_index_count(0)
                , max_elements_to_set(0)
                , ebo(0)
            {
                const u32 size_ebo = sizeof(index) * count;

                buffer.reserve(size_ebo);
                buffer.resize(size_ebo);

                opengl::api::instance().generate_buffers(1, &ebo);
                opengl::api::instance().bind_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                opengl::api::instance().buffer_data(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
                opengl::api::instance().bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            //-------------------------------------------------------------------------
            ~impl()
            {
                assert(ebo == 0 && "element buffer object not released");
            }

            //-------------------------------------------------------------------------
            impl(const impl& other) = delete;
            //-------------------------------------------------------------------------
            impl(impl&& other) noexcept = default;
            //-------------------------------------------------------------------------
            impl& operator=(const impl& other) = delete;
            //-------------------------------------------------------------------------
            impl& operator=(impl&& other) noexcept = default;

            //-------------------------------------------------------------------------
            void bind() const
            {
                opengl::api::instance().bind_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                opengl::api::instance().bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                if (current_index_count == previous_index_count)
                {
                    // No new indices have been added, skip upload
                    return;
                }

                constexpr u32 index_byte_size = sizeof(index);

                // Ensure that current_index_count hasn't decreased unexpectedly.
                assert(current_index_count >= previous_index_count && "Current index count decreased unexpectedly.");

                bind();

                const u32 buffer_offset = previous_index_count * index_byte_size;
                const u32 buffer_size = (current_index_count - previous_index_count) * index_byte_size;

                opengl::api::instance().buffer_sub_data(GL_ELEMENT_ARRAY_BUFFER, buffer_offset, buffer_size, buffer.data());
            }

            //-------------------------------------------------------------------------
            void free()
            {
                opengl::api::instance().delete_buffers(1, &ebo);
                ebo = 0;
            }

            //-------------------------------------------------------------------------
            u32                             previous_index_count;

            u32                             index_count;
            u32                             current_index_count;
            u32                             max_elements_to_set;

            std::vector<u8>                 buffer;

            u32                             ebo;
        };

        //-------------------------------------------------------------------------
        index_buffer::index_buffer(u32 index_count)
            : m_pimpl(std::make_unique<impl>(index_count))
        {}

        //-------------------------------------------------------------------------
        index_buffer::index_buffer(index_buffer&& other) noexcept
            :m_pimpl(std::exchange(other.m_pimpl, nullptr))
        {}

        //-------------------------------------------------------------------------
        index_buffer& index_buffer::operator=(index_buffer&& other) noexcept
        {
            m_pimpl = std::exchange(other.m_pimpl, nullptr);

            return *this;
        }

        //-------------------------------------------------------------------------
        index_buffer::~index_buffer() = default;

        // -------------------------------------------
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

            m_pimpl->previous_index_count = m_pimpl->current_index_count;
        }

        //-------------------------------------------------------------------------
        bool index_buffer::can_add(u32 max_elements_to_set) const
        {
            const u32 new_index_count = active_element_count() + max_elements_to_set;

            // Make sure we do not exceed the memory size of one block
            // Make sure we do not exceed the amount of indices we can store
            return new_index_count < m_pimpl->index_count;
        }
    
        //-------------------------------------------------------------------------
        void index_buffer::open(u32 max_elements_to_set) const
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }
        
        //-------------------------------------------------------------------------
        void index_buffer::close() const
        {
            m_pimpl->current_index_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;
        }

        //-------------------------------------------------------------------------
        void index_buffer::reset() const
        {
            m_pimpl->previous_index_count = 0;
            m_pimpl->current_index_count = 0;
        }

        //-------------------------------------------------------------------------
        void index_buffer::free() const
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
        u64 index_buffer::total_buffer_size_in_bytes() const
        {
            return sizeof(m_pimpl->buffer[0]) * m_pimpl->current_index_count;
        }

        //-------------------------------------------------------------------------
        u64 index_buffer::element_size_in_bytes() const
        {
            return sizeof(m_pimpl->buffer[0]);
        }

        //-------------------------------------------------------------------------
        u32 index_buffer::element_count() const
        {
            return m_pimpl->index_count;
        }

        //-------------------------------------------------------------------------
        u32 index_buffer::active_element_count() const
        {
            return m_pimpl->current_index_count;
        }
    }
}
