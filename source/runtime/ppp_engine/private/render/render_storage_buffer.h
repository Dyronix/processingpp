#pragma once

#include "util/types.h"

#include <vector>
#include <functional>
#include <memory>

namespace ppp
{
    namespace render
    {
        class storage_buffer
        {
        public:
            storage_buffer(u64 element_count, u64 element_size);
            ~storage_buffer();

        public:
            void                            bind(u32 binding_point) const;
            void                            unbind() const;
            void                            submit(u32 binding_point) const;

        public:
            bool                            can_add(u64 max_elements_to_set) const;

            void                            open(u64 max_elements_to_set);
            void                            close();

        public:
            void                            reset();
            void                            free();

            u8*                             data();
            const u8*                       data() const;

            u64                             total_size_in_bytes() const;
            u64                             element_size_in_bytes() const;
            u64                             element_count() const;
            u64                             active_element_count() const;

        private:
            class impl;
            std::unique_ptr<impl>           m_pimpl;
        };
    }
}