#pragma once

#include "util/types.h"

#include <unordered_map>
#include <vector>

namespace ppp
{
    namespace render
    {
        class texture_registry
        {
        public:
            texture_registry(s32 size_textures);

            bool can_add(s32 nr_textures);
            bool has_data() const;
            bool has_reserved_texture_space() const;

            s32 add_texture(u32 image_id);

            void reset();

            const std::vector<s32>& samplers() const;
            const std::vector<u32>& textures() const;

            u64 active_sampler_count() const;
            u64 active_texture_count() const;

            u32 max_texture_count() const;

        private:
            std::unordered_map<u32, s32> m_image_to_sampler_map;

            std::vector<u32> m_images;
            std::vector<s32> m_samplers;

            const s32 m_max_texture_count;
        };
    }
}