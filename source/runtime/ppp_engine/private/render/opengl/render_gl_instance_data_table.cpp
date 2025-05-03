#include "render/render_batch_data_table.h"
#include "render/render_instance_data_table.h"
#include "render/render_features.h"

#include "render/helpers/render_vertex_layouts.h"

#include "render/opengl/render_gl_util.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        instance_data_table::instance_data_table(string::string_id shader_tag)
            :m_shader_tag(shader_tag)
        {}

        //-------------------------------------------------------------------------
        void instance_data_table::reset()
        {
            for (auto& pair : m_instances)
            {
                pair.second.reset();
            }
        }
        //-------------------------------------------------------------------------
        void instance_data_table::clear()
        {
            for (auto& pair : m_instances)
            {
                pair.second.release();
            }
        }
        //-------------------------------------------------------------------------
        void instance_data_table::append(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {
            if (m_instances.find(topology) == std::cend(m_instances))
            {
                m_instances.emplace(topology, instance_drawing_data(layouts(m_shader_tag), layout_count(m_shader_tag)));
            }

            m_instances.at(topology).append(item, color, world);
        }

        //-------------------------------------------------------------------------
        u64 instance_data_table::size() const
        {
            return m_instances.size();
        }
        //-------------------------------------------------------------------------
        bool instance_data_table::empty() const
        {
            return m_instances.empty() || std::none_of(std::cbegin(m_instances), std::cend(m_instances),
                [](const auto& pair)
            {
                return pair.second.has_drawing_data();
            });
        }

        //-------------------------------------------------------------------------
        bool instance_data_table::has_texture_support() const
        {
            for (u64 i = 0; i < layout_count(m_shader_tag); ++i)
            {
                if (layouts(m_shader_tag)[i].type == attribute_type::TEXCOORD)
                {
                    return true;
                }
            }

            return false;
        }

        //-------------------------------------------------------------------------
        bool instance_data_table::has_normal_support() const
        {
            for (u64 i = 0; i < layout_count(m_shader_tag); ++i)
            {
                if (layouts(m_shader_tag)[i].type == attribute_type::NORMAL)
                {
                    return true;
                }
            }

            return false;
        }
    }
}