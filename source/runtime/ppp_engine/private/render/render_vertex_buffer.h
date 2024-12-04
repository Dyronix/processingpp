#pragma once

#include "render/render_item_components.h"

#include "util/log.h"

#include <vector>
#include <functional>

namespace ppp
{
    namespace render
    {
        class vertex_buffer
        {
        public:
            vertex_buffer(const vertex_attribute_layout* layouts, u64 layout_count, u64 vertex_count);

        public:
            void                            bind() const;
            void                            unbind() const;

            void                            upload(const void* data, u64 size);     

        public:
            void                            open();
            void                            close();

        public:
            void                            free();

            bool                            has_layout(vertex_attribute_type type) const;
            const vertex_attribute_layout*  find_layout(vertex_attribute_type type) const;
            const vertex_attribute_layout*  layouts() const;
            const u64                       layout_count() const;

            std::vector<u8>&                data();
            const std::vector<u8>&          data() const;

            u64                             total_size_in_bytes() const;
            u64                             vertex_size_in_bytes() const;
            u64                             vertex_count() const;
            u64                             active_vertex_count() const;

        private:
            class impl;
            std::unique_ptr<impl>           m_pimpl;
        };
    }
}