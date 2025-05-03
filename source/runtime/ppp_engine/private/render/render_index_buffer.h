#pragma once

#include "util/types.h"

#include <memory>

namespace ppp
{
    namespace render
    {
        class index_buffer
        {
        public:
            explicit index_buffer(u32 index_count);
            ~index_buffer();

            index_buffer(const index_buffer& other) = delete;
            index_buffer(index_buffer&& other) noexcept;

            index_buffer& operator=(const index_buffer& other) = delete;
            index_buffer& operator=(index_buffer&& other) noexcept;

        public:
            void                            bind() const;
            void                            unbind() const;
            void                            submit() const;

        public:
            bool                            can_add(u32 max_elements_to_set) const;

            void                            open(u32 max_elements_to_set) const;
            void                            close() const;

        public:
            void                            reset() const;
            void                            free() const;

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
