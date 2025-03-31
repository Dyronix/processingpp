#include "render/render_batch_data_table.h"
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
        batch_data_table::batch_data_table(string::string_id shader_tag)
            :m_shader_tag(shader_tag)
        {

        }

        //-------------------------------------------------------------------------
        void batch_data_table::reset()
        {
            for (auto& pair : m_batches)
            {
                pair.second.reset();
            }
        }
        //-------------------------------------------------------------------------
        void batch_data_table::clear()
        {
            for (auto& pair : m_batches)
            {
                pair.second.release();
            }
            m_batches.clear();
        }
        //-------------------------------------------------------------------------
        void batch_data_table::append(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {
            auto it = m_batches.find(topology);
            if (it == m_batches.end())
            {
                // Create a new batch_drawing_data if it doesn't exist.
                s32 max_ver = max_vertices(topology);
                s32 max_idx = max_indices(topology);
                auto emplaceResult = m_batches.emplace(topology, batch_drawing_data(max_ver, max_idx, internal::layouts(m_shader_tag), internal::layout_count(m_shader_tag)));
                it = emplaceResult.first;
            }
            it->second.append(item, color, world);
        }

        //-------------------------------------------------------------------------
        u64 batch_data_table::size() const
        {
            return m_batches.size();
        }
        //-------------------------------------------------------------------------
        bool batch_data_table::empty() const
        {
            return m_batches.empty() || std::none_of(std::cbegin(m_batches), std::cend(m_batches),
                [](const auto& pair)
            {
                return pair.second.has_drawing_data();
            });
        }

        //-------------------------------------------------------------------------
        bool batch_data_table::has_texture_support() const
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

        //-------------------------------------------------------------------------
        bool batch_data_table::has_normal_support() const
        {
            for (u64 i = 0; i < internal::layout_count(m_shader_tag); ++i)
            {
                if (internal::layouts(m_shader_tag)[i].type == attribute_type::NORMAL)
                {
                    return true;
                }
            }

            return false;
        }
    }
}