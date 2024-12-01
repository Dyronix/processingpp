#include "render/render_texture_manager.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        texture_manager::texture_manager(s32 size_textures)
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
        bool texture_manager::can_add(s32 nr_textures)
        {
            return has_reserved_texture_space() && m_image_to_sampler_map.size() + nr_textures < m_max_texture_count;
        }

        //-------------------------------------------------------------------------
        bool texture_manager::has_data() const
        {
            return m_images.size() > 0;
        }

        //-------------------------------------------------------------------------
        bool texture_manager::has_reserved_texture_space() const
        {
            return m_max_texture_count > 0;
        }

        //-------------------------------------------------------------------------
        s32 texture_manager::add_texture(u32 image_id)
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
        void texture_manager::reset()
        {
            m_image_to_sampler_map.clear();

            m_images.clear();
            m_samplers.clear();
        }

        //-------------------------------------------------------------------------
        const std::vector<s32>& texture_manager::samplers() const { return m_samplers; }
        const std::vector<u32>& texture_manager::textures() const { return m_images; }

        //-------------------------------------------------------------------------
        u64 texture_manager::active_sampler_count() const { return has_reserved_texture_space() ? m_samplers.size() : 0; }
        u64 texture_manager::active_texture_count() const { return has_reserved_texture_space() ? m_images.size() : 0; }

        //-------------------------------------------------------------------------
        u32 texture_manager::max_texture_count() const { return has_reserved_texture_space() ? m_max_texture_count : 0; }
    }
}