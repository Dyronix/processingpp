#include "render/render_index_buffer.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "memory/memory_types.h"
#include "memory/memory_unique_ptr_util.h"

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
                , previous_index_count(0)
                , max_elements_to_set(0)
                , buffer()
                , ebo(0)
            {
                const size_t size_ebo = sizeof(index) * count;

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
                if (ebo)
                {
                    opengl::api::instance().delete_buffers(1, &ebo);
                }
            }

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

                const u64 index_byte_size = sizeof(index);

                // Ensure that current_index_count hasn't decreased unexpectedly.
                assert(current_index_count >= previous_index_count && "Current index count decreased unexpectedly.");

                bind();

                u64 buffer_offset = previous_index_count * index_byte_size;
                u64 buffer_size = (current_index_count - previous_index_count) * index_byte_size;

                opengl::api::instance().buffer_sub_data(GL_ELEMENT_ARRAY_BUFFER, buffer_offset, buffer_size, buffer.data());
            }

            //-------------------------------------------------------------------------
            void free()
            {
                opengl::api::instance().delete_buffers(1, &ebo);
                ebo = 0;
            }

            //-------------------------------------------------------------------------
            u64                             previous_index_count;

            u64                             index_count;
            u64                             current_index_count;
            u64                             max_elements_to_set;

            stage_vector<u8>                buffer;

            u32                             ebo;
        };

        //-------------------------------------------------------------------------
        index_buffer::index_buffer(u64 index_count)
            : m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(index_count))
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

            m_pimpl->previous_index_count = m_pimpl->current_index_count;
        }

        //-------------------------------------------------------------------------
        bool index_buffer::can_add(u64 max_elements_to_set) const
        {
            using memory_policy = typename decltype(m_pimpl->buffer)::allocator_type::memory_policy;

            auto heap = memory_policy::get_heap();
            auto available_memory = heap->block_total_size();

            u64 new_index_count = active_index_count() + max_elements_to_set;
            u64 indices_size = sizeof(index) * new_index_count;

            // Make sure we do not exceed the memory size of one block
            // Make sure we do not exceed the amount of indices we can store
            return indices_size < available_memory && new_index_count < m_pimpl->index_count;
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
