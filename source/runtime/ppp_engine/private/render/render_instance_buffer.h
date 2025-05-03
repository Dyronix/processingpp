#pragma once

#include "render/render_item.h"

#include "util/log.h"

#include <functional>

namespace ppp
{
    namespace render
    {
        class instance_buffer
        {
        public:
            explicit instance_buffer(u32 instance_count, const attribute_layout* layouts, u32 layout_count, u32 layout_id_offset = 0);
            ~instance_buffer();

            instance_buffer(const instance_buffer& other) = delete;
            instance_buffer(instance_buffer&& other) noexcept;

            instance_buffer& operator=(const instance_buffer& other) = delete;
            instance_buffer& operator=(instance_buffer&& other) noexcept;

        public:
            void                            bind() const;
            void                            unbind() const;
            void                            submit() const;

        public:
            void                            open(u32 max_elements_to_set) const;
            void                            close() const;

        public:
            void                            reset() const;
            void                            free() const;

            bool                            has_layout(attribute_type type) const;
            const attribute_layout*         find_layout(attribute_type type) const;
            const attribute_layout*         layouts() const;
            u32                             layout_count() const;

            u8*                             data();
            const u8*                       data() const;

            u64                             total_buffer_size_in_bytes() const;
            u64                             element_size_in_bytes() const;
            u32                             element_count() const;
            u32                             active_element_count() const;

        private:
            class impl;
            std::unique_ptr<impl>           m_pimpl;
        };
    }
}