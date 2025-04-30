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
            index_buffer(u64 index_count);
            ~index_buffer();

        public:
            void                            bind() const;
            void                            unbind() const;
            void                            submit() const;

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
            u64                             index_size_in_bytes() const;
            u64                             index_count() const;
            u64                             active_index_count() const;

        private:
            class impl;
            std::unique_ptr<impl>           m_pimpl;
        };
    }
}
