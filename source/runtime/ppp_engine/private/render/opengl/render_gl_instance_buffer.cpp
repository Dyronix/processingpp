#include "render/render_instance_buffer.h"

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

        struct instance_buffer::impl
        {
            //-------------------------------------------------------------------------
            impl(const attribute_layout* layouts, u64 layout_count, u64 instance_count)
                : layouts(layouts)
                , layout_count(layout_count)
                , instance_count(instance_count)
                , current_instance_count(0)
                , max_elements_to_set(0)
                , buffer()
                , ibo(0)
                , is_bound(false)
            {
                auto instance_size = calculate_total_size_layout(layouts, layout_count);

                buffer.resize(instance_size * instance_count);

                GL_CALL(glGenBuffers(1, &ibo));
                GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, ibo));
                GL_CALL(glBufferData(GL_ARRAY_BUFFER, instance_count * instance_size, nullptr, GL_DYNAMIC_DRAW)); // Allocate initial size

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
                if (ibo)
                {
                    glDeleteBuffers(1, &ibo);
                }
            }

            //-------------------------------------------------------------------------
            void bind() const
            {
                glBindBuffer(GL_ARRAY_BUFFER, ibo);
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
                glDeleteBuffers(1, &ibo);
                ibo = 0;
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                assert(is_bound && "Cannot upload data to an unbound buffer");

                const u64 instance_size = calculate_total_size_layout(layouts, layout_count);

                GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, instance_size * instance_count, buffer.data()));
            }

            //-------------------------------------------------------------------------
            const attribute_layout*         layouts;
            const u64                       layout_count;

            u64                             instance_count;
            u64                             current_instance_count;
            u64                             max_elements_to_set;

            std::vector<u8>                 buffer;

            u32                             ibo;

            mutable bool                    is_bound;
        };

        //-------------------------------------------------------------------------
        instance_buffer::instance_buffer(const attribute_layout* layouts, u64 layout_count, u64 vertex_count)
            : m_pimpl(std::make_unique<impl>(layouts, layout_count, vertex_count))
        {

        }

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
        }

        //-------------------------------------------------------------------------
        void instance_buffer::open(u64 max_elements_to_set)
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }

        //-------------------------------------------------------------------------
        void instance_buffer::close()
        {
            m_pimpl->current_instance_count += m_pimpl->max_elements_to_set;
            m_pimpl->max_elements_to_set = 0;

            if (m_pimpl->instance_count == m_pimpl->current_instance_count)
            {
                s32 new_cap = m_pimpl->buffer.capacity() * 2;
                u64 layout_size = calculate_total_size_layout(layouts(), layout_count());

                m_pimpl->buffer.reserve(new_cap);

                glBufferData(GL_ARRAY_BUFFER, new_cap * layout_size, nullptr, GL_DYNAMIC_DRAW); // Allocate new GPU memory
            }
        }

        //-------------------------------------------------------------------------
        void instance_buffer::reset()
        {
            m_pimpl->current_instance_count = 0;
        }

        //-------------------------------------------------------------------------
        void instance_buffer::free()
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
        const u64 instance_buffer::layout_count() const
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
        u64 instance_buffer::total_size_in_bytes() const
        {
            auto instance_size = calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
            return instance_size * m_pimpl->current_instance_count;
        }

        //-------------------------------------------------------------------------
        u64 instance_buffer::instance_size_in_bytes() const
        {
            return calculate_total_size_layout(m_pimpl->layouts, m_pimpl->layout_count);
        }

        //-------------------------------------------------------------------------
        u64 instance_buffer::instance_count() const
        {
            return m_pimpl->instance_count;
        }

        //-------------------------------------------------------------------------
        u64 instance_buffer::active_instance_count() const
        {
            return m_pimpl->current_instance_count;
        }

    } // namespace render
} // namespace ppp
