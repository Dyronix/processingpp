#include "render/render_batch_data_table.h"
#include "render/render_instance_data_table.h"
#include "render/render_features.h"
#include "render/helpers/render_vertex_layouts.h"

#include "resources/shader_pool.h"

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static const attribute_layout* get_attribute_layout(string::string_id shader_tag)
            {
                auto shader_program = shader_pool::get_shader_program(shader_tag);

                return fill_user_layout(shader_program->vertex_format());
            }
            //-------------------------------------------------------------------------
            static u64 get_attribute_layout_count(string::string_id shader_tag)
            {
                auto shader_program = shader_pool::get_shader_program(shader_tag);

                return fill_user_layout_count(shader_program->vertex_format());
            }

            //-------------------------------------------------------------------------
            static const attribute_layout* layouts(string::string_id shader_tag)
            {
                return get_attribute_layout(shader_tag);
            }
            //-------------------------------------------------------------------------
            static u64 layout_count(string::string_id shader_tag)
            {
                return get_attribute_layout_count(shader_tag);
            }
        }

        //-------------------------------------------------------------------------
        instance_data_table::instance_data_table(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag)
            :m_shader_tag(shader_tag)
            , m_instance_layouts(instance_layouts)
            , m_instance_layout_count(instance_layout_count)
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
                m_instances.emplace(topology, instance_drawing_data(internal::layouts(m_shader_tag), internal::layout_count(m_shader_tag), m_instance_layouts, m_instance_layout_count));
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
            for (u64 i = 0; i < internal::layout_count(m_shader_tag); ++i)
            {
                if (internal::layouts(m_shader_tag)[i].type == attribute_type::TEXCOORD)
                {
                    return true;
                }
            }

            return false;
        }
    }
}