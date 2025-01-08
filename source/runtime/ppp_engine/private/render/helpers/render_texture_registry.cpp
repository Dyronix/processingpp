#include "render/helpers/render_texture_registry.h"

#include <algorithm>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static const std::vector<image_usage_type>& image_usage_types()
            {
                static std::vector<image_usage_type> all_types =
                {
                    image_usage_type::DIFFUSE, image_usage_type::NORMAL, image_usage_type::SPECULAR, image_usage_type::EMISSIVE,
                    image_usage_type::HEIGHT, image_usage_type::SHADOW, image_usage_type::CUSTOM_0, image_usage_type::CUSTOM_1
                };

                return all_types;
            }
        }

        //-------------------------------------------------------------------------
        // Texture Binding Map
        texture_binding_map::texture_binding_map(s32 size_textures)
            :m_max_texture_entries(size_textures)
        {
            if (size_textures != -1)
            {
                m_texture_entry_to_sampler_entry_map.reserve(size_textures);

                m_texture_entries.reserve(size_textures);
                m_sampler_entries.reserve(size_textures);
            }
        }

        //-------------------------------------------------------------------------
        bool texture_binding_map::can_add(s32 nr_textures) const
        {
            return has_reserved_texture_space() && m_texture_entry_to_sampler_entry_map.size() + nr_textures < m_max_texture_entries;
        }

        //-------------------------------------------------------------------------
        bool texture_binding_map::has_data() const
        {
            return m_texture_entries.size() > 0;
        }

        //-------------------------------------------------------------------------
        bool texture_binding_map::has_reserved_texture_space() const
        {
            return m_max_texture_entries > 0;
        }

        //-------------------------------------------------------------------------
        s32 texture_binding_map::add_texture(texture_entry entry)
        {
            if (has_reserved_texture_space())
            {
                if (m_texture_entry_to_sampler_entry_map.find(entry) == m_texture_entry_to_sampler_entry_map.cend())
                {
                    m_texture_entry_to_sampler_entry_map[entry] = m_sampler_entries.size();

                    m_texture_entries.push_back(entry);
                    m_sampler_entries.push_back(m_sampler_entries.size());
                }

                return m_texture_entry_to_sampler_entry_map.at(entry);
            }

            return -1;
        }

        //-------------------------------------------------------------------------
        void texture_binding_map::reset()
        {
            m_texture_entry_to_sampler_entry_map.clear();

            m_texture_entries.clear();
            m_sampler_entries.clear();
        }

        //-------------------------------------------------------------------------
        void texture_binding_map::release()
        {
            reset();
        }

        //-------------------------------------------------------------------------
        const std::vector<texture_sampler_entry>& texture_binding_map::samplers() const { return m_sampler_entries; }
        const std::vector<texture_entry>& texture_binding_map::textures() const { return m_texture_entries; }

        //-------------------------------------------------------------------------
        u64 texture_binding_map::active_sampler_count() const { return has_reserved_texture_space() ? m_sampler_entries.size() : 0; }
        u64 texture_binding_map::active_texture_count() const { return has_reserved_texture_space() ? m_texture_entries.size() : 0; }

        //-------------------------------------------------------------------------
        u32 texture_binding_map::max_texture_count() const { return has_reserved_texture_space() ? m_max_texture_entries : 0; }

        //-------------------------------------------------------------------------
        // Texture Registry
        
        //-------------------------------------------------------------------------
        texture_registry::texture_registry(s32 max_textures)
            :m_max_texture_count(max_textures)
        {
            s32 nr_types = (s32)internal::image_usage_types().size();

            m_binding_map.emplace(image_usage_type::DIFFUSE, texture_binding_map(max_textures / nr_types));
            m_binding_map.emplace(image_usage_type::SPECULAR, texture_binding_map(max_textures / nr_types));
            m_binding_map.emplace(image_usage_type::NORMAL, texture_binding_map(max_textures / nr_types));
            m_binding_map.emplace(image_usage_type::EMISSIVE, texture_binding_map(max_textures / nr_types));
            m_binding_map.emplace(image_usage_type::HEIGHT, texture_binding_map(max_textures / nr_types));
            m_binding_map.emplace(image_usage_type::SHADOW, texture_binding_map(max_textures / nr_types));
            m_binding_map.emplace(image_usage_type::CUSTOM_0, texture_binding_map(max_textures / nr_types));
            m_binding_map.emplace(image_usage_type::CUSTOM_1, texture_binding_map(max_textures / nr_types));
        }

        //-------------------------------------------------------------------------
        bool texture_registry::can_add_texture(image_usage_type usage_type, s32 nr_textures) const
        {
            if (m_binding_map.find(usage_type) != m_binding_map.cend())
            {
                return m_binding_map.at(usage_type).can_add(nr_textures);
            }

            return false;
        }

        //-------------------------------------------------------------------------
        bool texture_registry::has_data() const
        {
            return std::any_of(m_binding_map.begin(), m_binding_map.end(),
                [](const auto& pair)
            {
                return pair.second.has_data();
            });
        }

        //-------------------------------------------------------------------------
        bool texture_registry::has_reserved_texture_space() const
        {
            return std::any_of(m_binding_map.begin(), m_binding_map.end(),
                [](const auto& pair)
            {
                return pair.second.has_reserved_texture_space();
            });
        }

        //-------------------------------------------------------------------------
        s32 texture_registry::add_texture(image_usage_type usage_type, texture_entry entry)
        {
            if (m_binding_map.find(usage_type) != m_binding_map.cend())
            {
                return m_binding_map.at(usage_type).add_texture(entry);
            }

            return -1;
        }

        //-------------------------------------------------------------------------
        void texture_registry::reset()
        {
            for (auto& registry : m_binding_map)
            {
                registry.second.reset();
            }
        }

        //-------------------------------------------------------------------------
        void texture_registry::release()
        {
            for (auto& registry : m_binding_map)
            {
                registry.second.release();
            }
        }

        //-------------------------------------------------------------------------
        const std::vector<texture_sampler_entry>& texture_registry::samplers(image_usage_type usage_type) const
        {
            if (m_binding_map.find(usage_type) != m_binding_map.cend())
            {
                return m_binding_map.at(usage_type).samplers();
            }

            static std::vector<texture_sampler_entry> empty_vec;

            return empty_vec;
        }
        //-------------------------------------------------------------------------
        const std::vector<texture_entry>& texture_registry::textures(image_usage_type usage_type) const
        {
            if (m_binding_map.find(usage_type) != m_binding_map.cend())
            {
                return m_binding_map.at(usage_type).textures();
            }

            static std::vector<texture_entry> empty_vec;

            return empty_vec;
        }

        //-------------------------------------------------------------------------
        u64 texture_registry::active_sampler_count(image_usage_type usage_type) const
        {
            if (m_binding_map.find(usage_type) != m_binding_map.cend())
            {
                return m_binding_map.at(usage_type).active_sampler_count();
            }

            return 0;
        }
        //-------------------------------------------------------------------------
        u64 texture_registry::active_texture_count(image_usage_type usage_type) const
        {
            if (m_binding_map.find(usage_type) != m_binding_map.cend())
            {
                return m_binding_map.at(usage_type).active_texture_count();
            }

            return 0;
        }

        //-------------------------------------------------------------------------
        u32 texture_registry::max_texture_count() const
        {
            return has_reserved_texture_space() ? m_max_texture_count : 0;
        }
    }
}