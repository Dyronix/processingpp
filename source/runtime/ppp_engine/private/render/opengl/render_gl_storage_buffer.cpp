#include "render/render_storage_buffer.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "util/pointer_math.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        class storage_buffer::impl
        {
        public:
            //------------------------------------------------------------------------
            explicit impl(u32 in_element_count, u64 in_element_size, u32 bp)
                : previous_element_count(0)
                , element_size(memory::align_up(in_element_size, 16))
                , element_count(in_element_count)
                , current_element_count(0)
                , max_elements_to_set(0)
                , ssbo(0)
                , binding_point(bp)
            {
                const u64 size_ssbo = in_element_size * in_element_count;
                assert(size_ssbo == static_cast<u32>(size_ssbo));

                buffer.reserve(element_size * in_element_count);
                buffer.resize(element_size * in_element_count);

                opengl::api::instance().generate_buffers(1, &ssbo);
                opengl::api::instance().bind_buffer(GL_SHADER_STORAGE_BUFFER, ssbo);
                opengl::api::instance().buffer_data(GL_SHADER_STORAGE_BUFFER, static_cast<u32>(size_ssbo), nullptr, GL_DYNAMIC_DRAW);

                opengl::api::instance().bind_buffer(GL_SHADER_STORAGE_BUFFER, 0);
            }

            //-------------------------------------------------------------------------
            ~impl()
            {
                assert(ssbo == 0 && "storage buffer object not released");
            }

            //-------------------------------------------------------------------------
            impl(const impl& other) = delete;
            //-------------------------------------------------------------------------
            impl(impl&& other) noexcept = default;
            //-------------------------------------------------------------------------
            impl& operator=(const impl& other) = delete;
            //-------------------------------------------------------------------------
            impl& operator=(impl&& other) noexcept = default;

            //------------------------------------------------------------------------
            void bind() const
            {
                opengl::api::instance().bind_buffer_base(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo);
            }

            //------------------------------------------------------------------------
            void unbind() const
            {
                opengl::api::instance().bind_buffer_base(GL_SHADER_STORAGE_BUFFER, 0, 0);
            }

            //------------------------------------------------------------------------
            void resize_buffer(u32 new_element_count)
            {
                if (new_element_count > element_count)
                {
                    element_count = new_element_count;

                    const u64 size_ssbo = element_size * new_element_count;
                    assert(size_ssbo == static_cast<u32>(size_ssbo));

                    buffer.resize(size_ssbo);

                    opengl::api::instance().bind_buffer(GL_SHADER_STORAGE_BUFFER, ssbo);
                    opengl::api::instance().buffer_data(GL_SHADER_STORAGE_BUFFER, static_cast<u32>(size_ssbo), nullptr, GL_DYNAMIC_DRAW);
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
            void submit() const
            {
                if (current_element_count == previous_element_count)
                {
                    // No new elements have been added, skip upload
                    return;
                }

                // Ensure that current_element_count hasn't decreased unexpectedly.
                assert(current_element_count >= previous_element_count && "Current element count decreased unexpectedly.");

                bind();

                const u64 buffer_offset = previous_element_count * element_size;
                assert(buffer_offset == static_cast<u32>(buffer_offset));
                const u64 buffer_size = (current_element_count - previous_element_count) * element_size;
                assert(buffer_size == static_cast<u32>(buffer_size));

                opengl::api::instance().buffer_sub_data(
                    GL_SHADER_STORAGE_BUFFER,
                    static_cast<u32>(buffer_offset),
                    static_cast<u32>(buffer_size),
                    buffer.data());
            }

            //-------------------------------------------------------------------------
            u32 previous_element_count;

            u64 element_size;
            u32 element_count;
            u32 current_element_count;
            u32 max_elements_to_set;

            std::vector<u8> buffer;

            u32 ssbo;
            u32 binding_point;
        };

        //------------------------------------------------------------------------
        storage_buffer::storage_buffer(u32 element_count, u64 element_size, u32 binding_point)
            : m_pimpl(std::make_unique<impl>(element_count, element_size, binding_point))
        {

        }

        //-------------------------------------------------------------------------
        storage_buffer::storage_buffer(storage_buffer&& other) noexcept
            :m_pimpl(std::exchange(other.m_pimpl, nullptr))
        {}

        //-------------------------------------------------------------------------
        storage_buffer& storage_buffer::operator=(storage_buffer&& other) noexcept
        {
            m_pimpl = std::exchange(other.m_pimpl, nullptr);

            return *this;
        }

        //------------------------------------------------------------------------
        storage_buffer::~storage_buffer() = default;

        //------------------------------------------------------------------------
        void storage_buffer::bind() const
        {
            m_pimpl->bind();
        }

        //------------------------------------------------------------------------
        void storage_buffer::unbind() const
        {
            m_pimpl->unbind();
        }

        //------------------------------------------------------------------------
        void storage_buffer::submit() const
        {
            m_pimpl->submit();

            m_pimpl->previous_element_count = m_pimpl->current_element_count;
        }

        //-------------------------------------------------------------------------
        bool storage_buffer::can_add(u32 max_elements_to_set) const
        {
            return true;
        }

        //-------------------------------------------------------------------------
        void storage_buffer::open(u32 max_elements_to_set) const
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
            m_pimpl->resize_buffer(m_pimpl->current_element_count + max_elements_to_set);
        }

        //-------------------------------------------------------------------------
        void storage_buffer::close() const
        {
            m_pimpl->current_element_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;
        }

        //------------------------------------------------------------------------
        void storage_buffer::reset() const
        {
            m_pimpl->current_element_count = 0;
            m_pimpl->previous_element_count = 0;
        }

        //------------------------------------------------------------------------
        void storage_buffer::free() const
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
        u64 storage_buffer::total_buffer_size_in_bytes() const
        {
            return m_pimpl->current_element_count * m_pimpl->element_size;
        }

        //------------------------------------------------------------------------
        u64 storage_buffer::element_size_in_bytes() const
        {
            return m_pimpl->element_size;
        }

        //------------------------------------------------------------------------
        u32 storage_buffer::element_count() const
        {
            return m_pimpl->element_count;
        }

        //------------------------------------------------------------------------
        u32 storage_buffer::active_element_count() const
        {
            return m_pimpl->current_element_count;
        }

    } // namespace render
} // namespace ppp
