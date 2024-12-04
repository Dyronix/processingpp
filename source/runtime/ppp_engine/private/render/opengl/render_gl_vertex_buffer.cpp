#include "render/render_vertex_buffer.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        struct vertex_buffer::impl
        {
            //-------------------------------------------------------------------------
            impl(const vertex_attribute_layout* layouts, u64 layout_count, u64 vertex_count)
                : layouts(layouts)
                , layout_count(layout_count)
                , vertex_count(vertex_count)
                , current_vertex_count(0)
                , max_elements_to_set(0)
                , buffer()
                , vbo(0)
            {
                auto vertex_size = calculate_total_size_vertex_type(layouts, layout_count);

                buffer.resize(vertex_size * vertex_count);

                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertex_size * vertex_count, nullptr, GL_DYNAMIC_DRAW);
            }

            //-------------------------------------------------------------------------
            ~impl()
            {
                glDeleteBuffers(1, &vbo);
            }

            //-------------------------------------------------------------------------
            void upload(const void* data, size_t size) 
            {
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
            }

            //-------------------------------------------------------------------------
            void bind() const 
            {
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
            }

            //-------------------------------------------------------------------------
            void unbind() const 
            {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            //-------------------------------------------------------------------------
            const vertex_attribute_layout*  layouts;
            const u64                       layout_count;

            u64                             vertex_count;
            u64                             current_vertex_count;
            u64                             max_elements_to_set;
            
            std::vector<u8>                 buffer;

            u32                             vbo;
        };

        //-------------------------------------------------------------------------
        vertex_buffer::vertex_buffer(const vertex_attribute_layout* layouts, u64 layout_count, u64 vertex_count)
            : m_pimpl(std::make_unique<impl>(layouts, layout_count, vertex_count))
        {

        }

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
        void vertex_buffer::upload(const void* data, u64 size)
        {
            m_pimpl->upload(data, size);
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::open(u64 max_elements_to_set)
        {
            m_pimpl->max_elements_to_set = max_elements_to_set;
        }
        
        //-------------------------------------------------------------------------
        void vertex_buffer::close()
        {
            m_pimpl->max_elements_to_set = 0;
        }

        //-------------------------------------------------------------------------
        void vertex_buffer::free()
        {
            m_pimpl->current_vertex_count = 0;
        }

        //-------------------------------------------------------------------------
        const vertex_attribute_layout* vertex_buffer::find_layout(vertex_attribute_type type) const
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
        bool vertex_buffer::has_layout(vertex_attribute_type type) const
        {
            return find_layout(type) != nullptr;
        }

        //------------------------------------------------------------------------- 
        const vertex_attribute_layout* vertex_buffer::layouts() const
        {
            return m_pimpl->layouts;
        }
        
        //-------------------------------------------------------------------------
        const u64 vertex_buffer::layout_count() const
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
        u64 vertex_buffer::total_size_in_bytes() const
        {
            auto vertex_size = calculate_total_size_vertex_type(m_pimpl->layouts, m_pimpl->layout_count);
            return vertex_size * m_pimpl->current_vertex_count;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::vertex_size_in_bytes() const
        {
            return calculate_total_size_vertex_type(m_pimpl->layouts, m_pimpl->layout_count);
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::vertex_count() const
        {
            return m_pimpl->vertex_count;
        }

        //-------------------------------------------------------------------------
        u64 vertex_buffer::active_vertex_count() const
        {
            return m_pimpl->current_vertex_count;
        }

    } // namespace render
} // namespace ppp
