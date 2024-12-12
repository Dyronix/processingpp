#pragma once

#include "render/render_item_components.h"

#include "util/log.h"

#include <vector>
#include <functional>

namespace ppp
{
    namespace render
    {
        class instance_buffer
        {
        public:
            instance_buffer(u64 instance_count, const attribute_layout* layouts, u64 layout_count, u64 layout_id_offset = 0);
            ~instance_buffer();

        public:
            void                            bind() const;
            void                            unbind() const;
            void                            submit() const;

        public:
            void                            open(u64 max_elements_to_set);
            void                            close();

        public:
            void                            reset();
            void                            free();

            bool                            has_layout(attribute_type type) const;
            const attribute_layout*         find_layout(attribute_type type) const;
            const attribute_layout*         layouts() const;
            const u64                       layout_count() const;

            u8*                             data();
            const u8*                       data() const;

            u64                             total_size_in_bytes() const;
            u64                             instance_size_in_bytes() const;
            u64                             instance_count() const;
            u64                             active_instance_count() const;

        private:
            class impl;
            std::unique_ptr<impl>           m_pimpl;
        };
    }
}