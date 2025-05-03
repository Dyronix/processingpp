#pragma once

#include "util/types.h"

#include <memory>

namespace ppp
{
    namespace render
    {
        class storage_buffer
        {
        public:
            explicit storage_buffer(u32 element_count, u64 element_size, u32 binding_point);
            ~storage_buffer();

            storage_buffer(const storage_buffer& other) = delete;
            storage_buffer(storage_buffer&& other) noexcept;

            storage_buffer& operator=(const storage_buffer& other) = delete;
            storage_buffer& operator=(storage_buffer&& other) noexcept;

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