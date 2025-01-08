#pragma once

#include "util/types.h"

#include "render/render_types.h"

#include <unordered_map>
#include <vector>

namespace ppp
{
    namespace render
    {
        using texture_sampler_entry = s32;
        using texture_entry = texture_id;

        class texture_binding_map
        {
        public:
            texture_binding_map(s32 size_textures);

            bool can_add(s32 nr_textures) const;
            bool has_data() const;
            bool has_reserved_texture_space() const;

            s32 add_texture(texture_entry entry);

            void reset();
            void release();

            const std::vector<texture_sampler_entry>& samplers() const;
            const std::vector<texture_entry>& textures() const;

            u64 active_sampler_count() const;
            u64 active_texture_count() const;

            u32 max_texture_count() const;

        private:
            struct texture_entry_hasher
            {
                u64 operator()(texture_entry key) const
                {
                    // No hashing required for the texture_entry
                    return key;
                }
            };

            std::unordered_map<texture_entry, texture_sampler_entry, texture_entry_hasher> m_texture_entry_to_sampler_entry_map;

            std::vector<texture_entry> m_texture_entries;
            std::vector<texture_sampler_entry> m_sampler_entries;

            const s32 m_max_texture_entries;
        };

        class texture_registry
        {
        public:
            texture_registry(s32 size_textures);

            bool can_add_texture(image_usage_type usage_type, s32 nr_textures) const;
            bool has_data() const;
            bool has_reserved_texture_space() const;

            s32 add_texture(image_usage_type usage_type, texture_entry entry);

            void reset();
            void release();

            const std::vector<texture_sampler_entry>& samplers(image_usage_type usage_type) const;
            const std::vector<texture_entry>& textures(image_usage_type usage_type) const;

            u64 active_sampler_count(image_usage_type usage_type) const;
            u64 active_texture_count(image_usage_type usage_type) const;

            u32 max_texture_count() const;

        private:
            std::unordered_map<image_usage_type, texture_binding_map> m_binding_map;

            const s32 m_max_texture_count;
        };
    }
}