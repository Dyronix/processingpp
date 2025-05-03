#include "render/render_instance_buffer.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"
#include "render/opengl/render_gl_util.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        class instance_buffer::impl
        {
        public:
            //-------------------------------------------------------------------------
            explicit impl(u32 instance_count, const attribute_layout* layouts, u32 layout_count, u32 layout_id_offset)
                : previous_instance_count(0)
                , layouts(layouts)
                , layout_count(layout_count)
                , instance_count(instance_count)
                , current_instance_count(0)
                , max_elements_to_set(0)
                , ibo(0)
            {
                const u64 instance_size = calculate_total_size_layout(layouts, layout_count);
                const u64 buffer_size = instance_size * instance_count;
                assert(buffer_size == static_cast<u32>(buffer_size));

                buffer.reserve(instance_size * instance_count);
                buffer.resize(instance_size * instance_count);

                opengl::api::instance().generate_buffers(1, &ibo);
                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, ibo);
                opengl::api::instance().buffer_data(GL_ARRAY_BUFFER, static_cast<u32>(buffer_size), nullptr, GL_DYNAMIC_DRAW); // Allocate initial sie

                u64 attribute_index_offset = layout_id_offset;
                u64 attribute_stride_offset = 0;

                for (u64 i = 0; i < layout_count; ++i)
                {
                    const attribute_layout& layout = layouts[i];

                    for (s32 j = 0; j < layout.span; ++j)
                    {
                        const u64 attribute_index = attribute_index_offset + i + j;
                        assert(attribute_index == static_cast<u32>(attribute_index));
                        const u64 attribute_offset = attribute_stride_offset + j * layout.count * layout.element_size_in_bytes();
                        assert(attribute_offset == static_cast<u32>(attribute_offset));

                        opengl::api::instance().enable_vertex_attrib_array(static_cast<u32>(attribute_index));
                        switch (layout.data_type)
                        {
                        case attribute_data_type::FLOAT:
                            opengl::api::instance().vertex_attrib_pointer(static_cast<u32>(attribute_index), layout.count, gl_data_type(layout.data_type), layout.normalized ? GL_TRUE : GL_FALSE, layout.stride, (void*)attribute_offset);
                            break;
                        case attribute_data_type::UNSIGNED_INT: // fallthrough
                        case attribute_data_type::INT:
                            opengl::api::instance().vertex_attrib_i_pointer(static_cast<u32>(attribute_index), layout.count, gl_data_type(layout.data_type), layout.stride, (void*)attribute_offset);
                            break;
                        }

                        opengl::api::instance().vertex_attrib_divisor(static_cast<u32>(attribute_index), 1);
                    }

                    attribute_index_offset = layout_id_offset + (layout.span - 1);
                    attribute_stride_offset += layout.total_size_in_bytes();
                }

                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, 0);
            }
            //-------------------------------------------------------------------------
            ~impl()
            {
                assert(ibo == 0 && "instance buffer object not released");
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
                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, ibo);
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, 0);
            }

            //-------------------------------------------------------------------------
            void free()
            {
                opengl::api::instance().delete_buffers(1, &ibo);
                ibo = 0;
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                if (current_instance_count == previous_instance_count)
                {
                    // No new instances have been added, skip upload
                    return;
                }

                const u64 instance_size_byte_size = calculate_total_size_layout(layouts, layout_count);

                // Ensure that current_vertex_count hasn't decreased unexpectedly.
                assert(current_instance_count >= previous_instance_count && "Current instance count decreased unexpectedly.");

                bind();

                const u64 buffer_offset = previous_instance_count * instance_size_byte_size;
                assert(buffer_offset == static_cast<u32>(buffer_offset));
                const u64 buffer_size = (current_instance_count - previous_instance_count) * instance_size_byte_size;
                assert(buffer_size == static_cast<u32>(buffer_size));

                opengl::api::instance().buffer_sub_data(
                    GL_ARRAY_BUFFER,
                    static_cast<u32>(buffer_offset),
                    static_cast<u32>(buffer_size),
                    buffer.data());
            }

            //-------------------------------------------------------------------------
            u32                             previous_instance_count;

            const attribute_layout*         layouts;
            u32                             layout_count;

            u32                             instance_count;
            u32                             current_instance_count;
            u32                             max_elements_to_set;

            std::vector<u8>                 buffer;

            u32                             ibo;
        };

        //-------------------------------------------------------------------------
        instance_buffer::instance_buffer(u32 instance_count, const attribute_layout* layouts, u32 layout_count, u32 layout_id_offset)
            : m_pimpl(std::make_unique<impl>(instance_count, layouts, layout_count, layout_id_offset))
        {

        }

        //-------------------------------------------------------------------------
        instance_buffer::instance_buffer(instance_buffer&& other) noexcept
            :m_pimpl(std::exchange(other.m_pimpl, nullptr))
        {}

        //-------------------------------------------------------------------------
        instance_buffer& instance_buffer::operator=(instance_buffer&& other) noexcept
        {
            m_pimpl = std::exchange(other.m_pimpl, nullptr);

            return *this;
        }

        //-------------------------------------------------------------------------
        instance_buffer::~instance_buffer() = default;

        //-------------------------------------------------------------------------
        void instance_buffer::bind() const
        {
            m_pimpl->bind();
        }

        //-------------------------------------------------------------------------
        void instance_buffer::unbind() const
        {
            m_pimpl->unbind();
        }

        //-------------------------------------------------------------------------
        void instance_buffer::submit() const
        {
            m_pimpl->submit();

            m_pimpl->previous_instance_count = m_pimpl->current_instance_count;
        }

        //-------------------------------------------------------------------------
        void instance_buffer::open(u32 max_elements_to_set) const
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }

        //-------------------------------------------------------------------------
        void instance_buffer::close() const
        {
            m_pimpl->current_instance_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;

            if (m_pimpl->instance_count <= m_pimpl->current_instance_count)
            {
                const u64 new_size = m_pimpl->buffer.size() * 2u;
                const u64 layout_size = calculate_total_size_layout(layouts(), layout_count());
                const u64 buffer_size = new_size * layout_size;
                assert(buffer_size == static_cast<u32>(buffer_size));

                m_pimpl->buffer.resize(new_size);

                bind();

                opengl::api::instance().buffer_data(
                    GL_ARRAY_BUFFER,
                    static_cast<u32>(buffer_size),
                    nullptr,
                    GL_DYNAMIC_DRAW); // Allocate new GPU memory

                m_pimpl->instance_count = static_cast<s32>(new_size / layout_size);
            }
        }

        //-------------------------------------------------------------------------
        void instance_buffer::reset() const
        {
            m_pimpl->previous_instance_count = 0;
            m_pimpl->current_instance_count = 0;
        }

        //-------------------------------------------------------------------------
        void instance_buffer::free() const
        {
            reset();

            m_pimpl->buffer.clear();

            m_pimpl->unbind();
            m_pimpl->free();
        }

        //-------------------------------------------------------------------------
        const attribute_layout* instance_buffer::find_layout(attribute_type type) const
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
        bool instance_buffer::has_layout(attribute_type type) const
        {
            return find_layout(type) != nullptr;
        }

        //------------------------------------------------------------------------- 
        const attribute_layout* instance_buffer::layouts() const
        {
            return m_pimpl->layouts;
        }

        //-------------------------------------------------------------------------
        u32 instance_buffer::layout_count() const
        {
            return m_pimpl->layout_count;
        }

        //-------------------------------------------------------------------------
        u8* instance_buffer::data()
        {
            return m_pimpl->buffer.data();
        }

        //-------------------------------------------------------------------------
        const u8* instance_buffer::data() const
        {
            return m_pimpl->buffer.data();
        }

        //-------------------------------------------------------------------------
        u64 instance_buffer::total_buffer_size_in_bytes() const
        {
            auto instance_size = calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
            return instance_size * m_pimpl->current_instance_count;
        }

        //-------------------------------------------------------------------------
        u64 instance_buffer::element_size_in_bytes() const
        {
            return calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
        }

        //-------------------------------------------------------------------------
        u32 instance_buffer::element_count() const
        {
            return m_pimpl->instance_count;
        }

        //-------------------------------------------------------------------------
        u32 instance_buffer::active_element_count() const
        {
            return m_pimpl->current_instance_count;
        }

    } // namespace render
} // namespace ppp
