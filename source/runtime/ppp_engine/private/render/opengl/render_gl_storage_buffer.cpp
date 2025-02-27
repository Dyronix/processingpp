#include "render/render_storage_buffer.h"

#include "render/opengl/render_gl_error.h"

#include "memory/memory_unique_ptr_util.h"
#include "memory/memory_manager.h"

#include "util/pointer_math.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        struct storage_buffer::impl
        {
            //------------------------------------------------------------------------
            impl(u64 in_element_count, u64 in_element_size)
                : element_size(memory::align_up(in_element_size, 16))
                , element_count(in_element_count)
                , current_element_count(0)
                , previous_element_count(0)
                , max_elements_to_set(0)
                , buffer()
                , ssbo(0)
            {
                buffer.reserve(element_size * in_element_count);
                buffer.resize(element_size * in_element_count);

                GL_CALL(glGenBuffers(1, &ssbo));
                GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo));
                GL_CALL(glBufferData(GL_SHADER_STORAGE_BUFFER, element_size * in_element_count, nullptr, GL_DYNAMIC_DRAW));

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
            void resize_buffer(u64 new_element_count)
            {
                if (new_element_count > element_count)
                {
                    element_count = new_element_count;

                    buffer.resize(element_size * new_element_count);

                    GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo));
                    GL_CALL(glBufferData(GL_SHADER_STORAGE_BUFFER, element_size * new_element_count, nullptr, GL_DYNAMIC_DRAW));
                    GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
                }
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
                if (current_element_count == previous_element_count)
                {
                    // No new elements have been added, skip upload
                    return;
                }

                // Ensure that current_element_count hasn't decreased unexpectedly.
                assert(current_element_count >= previous_element_count && "Current vertex count decreased unexpectedly.");

                bind(binding_point);

                u64 buffer_offset = previous_element_count * element_size;
                u64 buffer_size = (current_element_count - previous_element_count) * element_size;

                GL_CALL(glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset, buffer_size, buffer.data()));
            }

            //-------------------------------------------------------------------------
            u64                             previous_element_count;

            u64                             element_size;
            u64                             element_count;
            u64                             current_element_count;
            u64                             max_elements_to_set;

            stage_vector<u8>                buffer;

            u32                             ssbo;
        };

        //------------------------------------------------------------------------
        storage_buffer::storage_buffer(u64 element_count, u64 element_size)
            : m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(element_count, element_size))
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

            m_pimpl->previous_element_count = m_pimpl->current_element_count;
        }

        //-------------------------------------------------------------------------
        bool storage_buffer::can_add(u64 max_elements_to_set) const
        {
            using memory_policy = typename decltype(m_pimpl->buffer)::allocator_type::memory_policy;

            auto heap = memory_policy::get_heap();
            auto block_total_size = heap->block_total_size(); // in bytes

            // Total number of elements after adding the new ones.
            u64 new_element_count = active_element_count() + max_elements_to_set;

            // Compute total bytes required (each element occupies m_pimpl->element_size bytes).
            u64 required_bytes = new_element_count * m_pimpl->element_size;

            // Current allocated capacity in bytes.
            u64 current_capacity_bytes = m_pimpl->buffer.capacity();

            // If the required bytes fit in the current allocation, no resize occurs.
            if (required_bytes <= current_capacity_bytes)
            {
                return true;
            }

            // Otherwise, predict the new capacity based on a doubling strategy.
            u64 predicted_capacity_bytes = (current_capacity_bytes == 0)
                ? required_bytes
                : current_capacity_bytes * 2;

            // Ensure the predicted capacity is at least enough for the required bytes.
            if (predicted_capacity_bytes < required_bytes)
            {
                predicted_capacity_bytes = required_bytes;
            }

            // Finally, check that the predicted new allocation stays within the block's total size.
            return predicted_capacity_bytes <= block_total_size;
        }

        //-------------------------------------------------------------------------
        void storage_buffer::open(u64 max_elements_to_set)
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
            m_pimpl->resize_buffer(m_pimpl->current_element_count + max_elements_to_set);
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
            m_pimpl->previous_element_count = 0;
        }

        //------------------------------------------------------------------------
        void storage_buffer::free()
        {
            m_pimpl->unbind();
            m_pimpl->free();

            m_pimpl->buffer.clear();
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
