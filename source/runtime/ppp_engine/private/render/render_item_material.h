#pragma once

#include "util/types.h"

#include "render/render_types.h"

#include <vector>
#include <unordered_map>
#include <algorithm>

namespace ppp
{
    namespace render
    {
        class material_attributes
        {
        public:
            material_attributes(u32 max_textures)
                : m_max_textures(max_textures)
            {
            }

            bool has_textures() const
            {
                return std::any_of(m_textures.cbegin(), m_textures.cend(),
                    [](const auto& pair)
                {
                    return !pair.second.empty();
                });
            }

            bool has_texture_id(texture_id texture_index) const
            {
                auto pred = [texture_index](const texture_id& id)
                {
                    return id == texture_index;
                };

                return std::any_of(m_textures.cbegin(), m_textures.cend(),
                    [&](const auto& pair)
                {
                    const auto& textures = pair.second;
                    return std::find_if(textures.cbegin(), textures.cend(), pred) != textures.cend();
                });
            }

            void add_texture(image_usage_type type, texture_id texture_index)
            {
                auto& textures = m_textures[type];
                if (textures.size() >= m_max_textures)
                {
                    throw std::runtime_error("Maximum textures exceeded for this type.");
                }
                textures.push_back(texture_index);
            }

            const std::vector<texture_id>& textures(image_usage_type type) const
            {
                static const std::vector<texture_id> empty_vector;

                auto it = m_textures.find(type);
                return it != m_textures.cend() ? it->second : empty_vector;
            }

            s32 num_textures(image_usage_type type) const
            {
                auto it = m_textures.find(type);
                return it != m_textures.cend() ? static_cast<s32>(it->second.size()) : 0;
            }

            void reset()
            {
                m_textures.clear();
            }

        private:
            std::unordered_map<image_usage_type, std::vector<texture_id>> m_textures;

            u32 m_max_textures;
        };
    }
}
