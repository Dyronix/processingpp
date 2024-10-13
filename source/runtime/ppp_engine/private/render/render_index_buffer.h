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

            bool can_add_indices(u64 count) const;

            void set_index_data(const u32* data_ptr, u64 data_count);

            void free();

            u32* get_data();
            const u32* get_data() const;

            u64 size_in_bytes() const;
            u64 index_size() const;
            u64 index_count() const;
            u64 active_index_count() const;

        private:
            u64 m_index_count;
            u64 m_current_index_count;

            std::unique_ptr<u32[]> m_buffer;
        };
    }
}
