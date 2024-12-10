#include "resources/texture_registry.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        texture_registry::texture_registry(s32 size_textures)
            :m_max_texture_count(size_textures)
        {
            if (size_textures != -1)
            {
                m_image_to_sampler_map.reserve(size_textures);

                m_images.reserve(size_textures);
                m_samplers.reserve(size_textures);
            }
        }

        //-------------------------------------------------------------------------
        bool texture_registry::can_add(s32 nr_textures)
        {
            return has_reserved_texture_space() && m_image_to_sampler_map.size() + nr_textures < m_max_texture_count;
        }

        //-------------------------------------------------------------------------
        bool texture_registry::has_data() const
        {
            return m_images.size() > 0;
        }

        //-------------------------------------------------------------------------
        bool texture_registry::has_reserved_texture_space() const
        {
            return m_max_texture_count > 0;
        }

        //-------------------------------------------------------------------------
        s32 texture_registry::add_texture(u32 image_id)
        {
            if (has_reserved_texture_space())
            {
                if (m_image_to_sampler_map.find(image_id) == m_image_to_sampler_map.cend())
                {
                    m_image_to_sampler_map[image_id] = m_samplers.size();

                    m_images.push_back(image_id);
                    m_samplers.push_back(m_samplers.size());
                }

                return m_image_to_sampler_map.at(image_id);
            }

            return -1;
        }

        //-------------------------------------------------------------------------
        void texture_registry::reset()
        {
            m_image_to_sampler_map.clear();

            m_images.clear();
            m_samplers.clear();
        }

        //-------------------------------------------------------------------------
        void texture_registry::release()
        {
            reset();
        }

        //-------------------------------------------------------------------------
        const std::vector<s32>& texture_registry::samplers() const { return m_samplers; }
        const std::vector<u32>& texture_registry::textures() const { return m_images; }

        //-------------------------------------------------------------------------
        u64 texture_registry::active_sampler_count() const { return has_reserved_texture_space() ? m_samplers.size() : 0; }
        u64 texture_registry::active_texture_count() const { return has_reserved_texture_space() ? m_images.size() : 0; }

        //-------------------------------------------------------------------------
        u32 texture_registry::max_texture_count() const { return has_reserved_texture_space() ? m_max_texture_count : 0; }
    }
}