#include "render/render_vertex_buffer.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"
#include "render/opengl/render_gl_util.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        class vertex_buffer::impl
        {
        public:
            //-------------------------------------------------------------------------
            explicit impl(u32 vertex_count, const attribute_layout* layouts, u32 layout_count, u32 layout_id_offset)
                : previous_vertex_count(0)
                , layouts(layouts)
                , layout_count(layout_count)
                , vertex_count(vertex_count)
                , current_vertex_count(0)
                , max_elements_to_set(0)
                , vbo(0)
            {
                const u64 vertex_size = calculate_total_size_layout(layouts, layout_count);
                const u64 buffer_size = vertex_size * vertex_count;
                assert(buffer_size == static_cast<u32>(buffer_size));

                buffer.reserve(vertex_count * vertex_size);
                buffer.resize(vertex_count * vertex_size);

                opengl::api::instance().generate_buffers(1, &vbo);
                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, vbo);
                opengl::api::instance().buffer_data(GL_ARRAY_BUFFER, static_cast<u32>(buffer_size), nullptr, GL_DYNAMIC_DRAW);

                u64 attribute_index_offset = layout_id_offset;
                u64 attribute_stride_offset = 0;

                for (u32 i = 0; i < layout_count; ++i)
                {
                    const attribute_layout& layout = layouts[i];

                    for (s32 j = 0; j < layout.span; ++j)
                    {
                        const u64 attribute_index = attribute_index_offset + i + j;
                        assert(attribute_index == static_cast<u32>(attribute_index));
                        const u64 attribute_offset = attribute_stride_offset + static_cast<u64>(j) * layout.count * layout.element_size_in_bytes();
                        assert(attribute_offset == static_cast<u32>(attribute_offset));

                        opengl::api::instance().enable_vertex_attrib_array(static_cast<u32>(attribute_index));
                        switch (layout.data_type)
                        {
                        case attribute_data_type::FLOAT:
                            opengl::api::instance().vertex_attrib_pointer(
                                static_cast<u32>(attribute_index),
                                layout.count,
                                gl_data_type(layout.data_type),
                                layout.normalized ? GL_TRUE : GL_FALSE,
                                layout.stride,
                                reinterpret_cast<void*>(attribute_offset));  // NOLINT(performance-no-int-to-ptr)
                            break;
                        case attribute_data_type::UNSIGNED_INT: // fallthrough
                        case attribute_data_type::INT:
                            opengl::api::instance().vertex_attrib_i_pointer(
                                static_cast<u32>(attribute_index),
                                layout.count,
                                gl_data_type(layout.data_type),
                                layout.stride,
                                reinterpret_cast<void*>(attribute_offset));  // NOLINT(performance-no-int-to-ptr)
                            break;
                        }
                    }

                    attribute_index_offset = layout_id_offset + (layout.span - 1);
                    attribute_stride_offset += layout.total_size_in_bytes();
                }
                
                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, 0);
            }
            //-------------------------------------------------------------------------
            ~impl()
            {
                assert(vbo == 0 && "array buffer object not released");
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
                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, vbo);
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                opengl::api::instance().bind_buffer(GL_ARRAY_BUFFER, 0);
            }

            //-------------------------------------------------------------------------
            void free()
            {
                opengl::api::instance().delete_buffers(1, &vbo);
                vbo = 0;
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                if (current_vertex_count == previous_vertex_count)
                {
                    // No new vertices have been added, skip upload
                    return;
                }

                const u64 vertex_buffer_byte_size = calculate_total_size_layout(layouts, layout_count);

                // Ensure that current_vertex_count hasn't decreased unexpectedly.
                assert(current_vertex_count >= previous_vertex_count && "Current vertex count decreased unexpectedly.");

                bind();

                const u64 buffer_offset = previous_vertex_count * vertex_buffer_byte_size;
                assert(buffer_offset == static_cast<u32>(buffer_offset));
                const u64 buffer_size = (current_vertex_count - previous_vertex_count) * vertex_buffer_byte_size;
                assert(buffer_size == static_cast<u32>(buffer_size));

                opengl::api::instance().buffer_sub_data(
                    GL_ARRAY_BUFFER,
                    static_cast<u32>(buffer_offset),
                    static_cast<u32>(buffer_size),
                    buffer.data());
            }

            //-------------------------------------------------------------------------
            u32                             previous_vertex_count;
            
            const attribute_layout*         layouts;
            u32                             layout_count;

            u32                             vertex_count;
            u32                             current_vertex_count;
            u32                             max_elements_to_set;
            
            std::vector<u8>                 buffer;

            u32                             vbo;
        };

        //-------------------------------------------------------------------------
        vertex_buffer::vertex_buffer(u32 vertex_count, const attribute_layout* layouts, u32 layout_count, u32 layout_id_offset)
            : m_pimpl(std::make_unique<impl>(vertex_count, layouts, layout_count, layout_id_offset))
        {

        }

        //-------------------------------------------------------------------------
        vertex_buffer::vertex_buffer(vertex_buffer&& other) noexcept
            :m_pimpl(std::exchange(other.m_pimpl, nullptr))
        {}

        //-------------------------------------------------------------------------
        vertex_buffer& vertex_buffer::operator=(vertex_buffer&& other) noexcept
        { 
            m_pimpl = std::exchange(other.m_pimpl, nullptr);

            return *this;
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
        bool vertex_buffer::can_add(u32 max_elements_to_set) const
        {
            u32 new_vertex_count = active_element_count() + max_elements_to_set;

            // Make sure we do not exceed the memory size of one block
            // Make sure we do not exceed the amount of vertices we can store
            return new_vertex_count < m_pimpl->vertex_count;
        }
    
        //-------------------------------------------------------------------------
        void vertex_buffer::open(u32 max_elements_to_set) const
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }
        
        //-------------------------------------------------------------------------
        void vertex_buffer::close() const
        {
            m_pimpl->current_vertex_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::reset() const
        {
            m_pimpl->previous_vertex_count = 0;
            m_pimpl->current_vertex_count = 0;
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::free() const
        {
            reset();

            m_pimpl->buffer.clear();

            m_pimpl->unbind();
            m_pimpl->free();
        }

        //-------------------------------------------------------------------------
        const attribute_layout* vertex_buffer::find_layout(attribute_type type) const
        {
            for (u32 i = 0; i < m_pimpl->layout_count; ++i)
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
        u32 vertex_buffer::layout_count() const
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
        u64 vertex_buffer::total_buffer_size_in_bytes() const
        {
            u64 vertex_size = calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
            return vertex_size * m_pimpl->current_vertex_count;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::element_size_in_bytes() const
        {
            return calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
        }

        //-------------------------------------------------------------------------
        u32 vertex_buffer::element_count() const
        {
            return m_pimpl->vertex_count;
        }

        //-------------------------------------------------------------------------
        u32 vertex_buffer::active_element_count() const
        {
            return m_pimpl->current_vertex_count;
        }

    } // namespace render
} // namespace ppp
