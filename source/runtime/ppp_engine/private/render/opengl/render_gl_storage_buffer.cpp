#include "render/render_storage_buffer.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

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

                opengl::api::instance().generate_buffers(1, &ssbo);
                opengl::api::instance().bind_buffer(GL_SHADER_STORAGE_BUFFER, ssbo);
                opengl::api::instance().buffer_data(GL_SHADER_STORAGE_BUFFER, element_size * in_element_count, nullptr, GL_DYNAMIC_DRAW);

                opengl::api::instance().bind_buffer(GL_SHADER_STORAGE_BUFFER, 0);
            }

            //------------------------------------------------------------------------
            ~impl()
            {
                if (ssbo)
                {
                    opengl::api::instance().delete_buffers(1, &ssbo);
                }
            }

            //------------------------------------------------------------------------
            void bind(u32 binding_point) const
            {
                opengl::api::instance().bind_buffer_base(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo);
            }

            //------------------------------------------------------------------------
            void unbind() const
            {
                opengl::api::instance().bind_buffer_base(GL_SHADER_STORAGE_BUFFER, 0, 0);
            }

            //------------------------------------------------------------------------
            void resize_buffer(u64 new_element_count)
            {
                if (new_element_count > element_count)
                {
                    element_count = new_element_count;

                    buffer.resize(element_size * new_element_count);

                    opengl::api::instance().bind_buffer(GL_SHADER_STORAGE_BUFFER, ssbo);
                    opengl::api::instance().buffer_data(GL_SHADER_STORAGE_BUFFER, element_size * new_element_count, nullptr, GL_DYNAMIC_DRAW);
                    opengl::api::instance().bind_buffer(GL_SHADER_STORAGE_BUFFER, 0);
                }
            }

            //------------------------------------------------------------------------
            void free()
            {
                opengl::api::instance().delete_buffers(1, &ssbo);

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

                opengl::api::instance().buffer_sub_data(GL_SHADER_STORAGE_BUFFER, buffer_offset, buffer_size, buffer.data());
            }

            //-------------------------------------------------------------------------
            u64 previous_element_count;

            u64 element_size;
            u64 element_count;
            u64 current_element_count;
            u64 max_elements_to_set;

            std::vector<u8> buffer;

            u32 ssbo;
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

            m_pimpl->previous_element_count = m_pimpl->current_element_count;
        }

        //-------------------------------------------------------------------------
        bool storage_buffer::can_add(u64 max_elements_to_set) const
        {
            return true;
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
