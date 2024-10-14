#include "render/render_batch.h"
#include "render/render_vertex_buffer.h"
#include "render/render_index_buffer.h"

#include "util/types.h"
#include "util/log.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            GLenum convert_to_gl_data_type(vertex_attribute_data_type type)
            {
                switch (type)
                {
                case vertex_attribute_data_type::FLOAT: return GL_FLOAT;
                case vertex_attribute_data_type::UNSIGNED_INT: return GL_UNSIGNED_INT;
                }
                return 0;  // Fallback to avoid compiler warnings
            }

            u64 calculate_total_size_vertex_type(const vertex_attribute_layout* layouts, u64 layout_count)
            {
                u64 total = 0;
                for (u64 i = 0; i < layout_count; ++i)
                {
                    const vertex_attribute_layout& layout = layouts[i];

                    s32 component_count = component_count_for_vertex_attribute(layout.type);
                    s32 size_in_bytes = size_in_bytes_for_data_type(layout.data_type);

                    total += (size_in_bytes * component_count);
                }
                return total;
            }
        }

        class batch_buffer_manager
        {
        public:
            batch_buffer_manager(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count)
                : m_max_vertex_count(size_vertex_buffer)
                , m_max_index_count(size_index_buffer)
                , m_vertex_buffer(layouts, layout_count, size_vertex_buffer)
                , m_index_buffer(size_index_buffer)
            {
                assert(size_vertex_buffer > 0);
                assert(size_index_buffer > 0);

                assert(layouts != nullptr);
                assert(layout_count > 0);
            }

            bool can_add(s32 nr_vertices, s32 nr_indices) const
            {
                return m_vertex_buffer.active_vertex_count() + nr_vertices < m_max_vertex_count && m_index_buffer.active_index_count() + nr_indices < m_max_index_count;
            }

            void add_vertices(const vertex_component* vertex_comp, const glm::vec4& color, const glm::mat4& world)
            {
                assert(vertex_comp != nullptr);

                s32 start_index = m_vertex_buffer.active_vertex_count();
                s32 end_index = start_index + vertex_comp->vertex_count();

                copy_vertex_data(vertex_comp);

                map_new_vertex_data(vertex_attribute_type::COLOR, (void*)&color[0]);

                transform_vertex_positions(start_index, end_index, world);
            }

            void add_vertices(const vertex_component* vertex_comp, s32 sampler_id, const glm::vec4& color, const glm::mat4& world)
            {
                assert(vertex_comp != nullptr);

                s32 start_index = m_vertex_buffer.active_vertex_count();
                s32 end_index = start_index + vertex_comp->vertex_count();

                add_vertices(vertex_comp, color, world);

                transform_vertex_diffuse_texture_ids(start_index, end_index, sampler_id);
            }

            void add_indices(const index_component* index_comp)
            {
                assert(index_comp != nullptr);

                s32 start_index = m_index_buffer.active_index_count();
                s32 end_index = start_index + index_comp->index_count();
                copy_index_data(index_comp);
                transform_index_locations(start_index, end_index, m_vertex_buffer.active_vertex_count());
            }

            void reset()
            {
                m_vertex_buffer.free();
                m_index_buffer.free();
            }

            u32 active_vertex_count() const { return m_vertex_buffer.active_vertex_count(); }
            u64 active_vertices_byte_size() const { return m_vertex_buffer.size_in_bytes(); }
            u32 active_index_count() const { return m_index_buffer.active_index_count(); }
            u64 active_indices_byte_size() const { return m_index_buffer.size_in_bytes(); }

            u32 max_vertex_count() const { return m_max_vertex_count; }
            u32 max_index_count() const { return m_max_index_count; }

            const void* vertices() const { return m_vertex_buffer.get_data().data(); }
            const void* indices() const { return m_index_buffer.get_data(); }

        private:
            void copy_vertex_data(const vertex_component* vertex_comp)
            {
                m_vertex_buffer.open_attribute_addition(vertex_comp->vertex_count());

                for (const auto& pair : vertex_comp->get_attribute_data())
                {
                    m_vertex_buffer.set_attribute_data(pair.first, pair.second.blob.data());
                }

                m_vertex_buffer.close_attribute_addition();
            }
            void copy_index_data(const index_component* index_comp)
            {
                u64 idx_count = index_comp->index_count();

                assert(sizeof(index_comp->indices()[0]) == sizeof(index) && "different index size was used");

                m_index_buffer.set_index_data(index_comp->indices(), idx_count);
            }

            void map_new_vertex_data(vertex_attribute_type type, void* data)
            {
                m_vertex_buffer.map_attribute_data(type, data);
            }

            void transform_vertex_positions(s32 start_index, s32 end_index, const glm::mat4& world)
            {
                m_vertex_buffer.transform_attribute_data<glm::vec3>(vertex_attribute_type::POSITION, start_index, end_index, [&](glm::vec3& position)
                {
                    glm::vec4 transformed_pos = world * glm::vec4(position, 1.0f);

                    position.x = transformed_pos.x;
                    position.y = transformed_pos.y;
                    position.z = transformed_pos.z;
                });
            }
            void transform_vertex_diffuse_texture_ids(s32 start_index, s32 end_index, s32 sampler_id)
            {
                m_vertex_buffer.transform_attribute_data<f32>(vertex_attribute_type::DIFFUSE_TEXTURE_INDEX, start_index, end_index, [&](float& id)
                {
                    id = (f32)sampler_id;
                });
            }
            void transform_index_locations(s32 start_index, s32 end_index, u64 offset)
            {
                m_index_buffer.transform_index_data(start_index, end_index, [&](u32& index)
                {
                    index += offset;
                });
            }

            const u32 m_max_vertex_count = 0;
            const u32 m_max_index_count = 0;

            vertex_buffer m_vertex_buffer;
            index_buffer m_index_buffer;
        };
        class batch_texture_manager
        {
        public:
            batch_texture_manager(s32 size_textures)
                :m_max_texture_count(size_textures)
            {
                if (size_textures != -1)
                {
                    m_image_to_sampler_map.reserve(size_textures);

                    m_images.reserve(size_textures);
                    m_samplers.reserve(size_textures);
                }
            }

            bool can_add(s32 nr_textures)
            {
                return has_reserved_texture_space() && m_image_to_sampler_map.size() + nr_textures < m_max_texture_count;
            }

            bool has_reserved_texture_space() const
            {
                return m_max_texture_count > 0;
            }

            s32 add_texture(u32 image_id)
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

            void reset()
            {
                m_image_to_sampler_map.clear();

                m_images.clear();
                m_samplers.clear();
            }

            const std::vector<s32>& samplers() const { return m_samplers; }
            const std::vector<u32>& textures() const { return m_images; }

            u64 active_sampler_count() const { return has_reserved_texture_space() ? m_samplers.size() : 0; }
            u64 active_texture_count() const { return has_reserved_texture_space() ? m_images.size() : 0; }

            u32 max_texture_count() const { return has_reserved_texture_space() ? m_max_texture_count : 0; }

        private:
            std::unordered_map<u32, s32> m_image_to_sampler_map;

            std::vector<u32> m_images;
            std::vector<s32> m_samplers;

            const u32 m_max_texture_count;
        };

        //-------------------------------------------------------------------------
        batch::batch(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count, s32 size_textures)
            : m_buffer_manager(std::make_unique<batch_buffer_manager>(size_vertex_buffer, size_index_buffer, layouts, layout_count))
            , m_texture_manager(std::make_unique<batch_texture_manager>(size_textures))
        {
        }

        //-------------------------------------------------------------------------
        batch::~batch() = default;

        //-------------------------------------------------------------------------
        batch::batch(batch&& other)
            : m_buffer_manager(std::exchange(other.m_buffer_manager, nullptr))
            , m_texture_manager(std::exchange(other.m_texture_manager, nullptr))
        {
        }

        //-------------------------------------------------------------------------
        batch& batch::operator=(batch&& other)
        {
            m_buffer_manager = std::exchange(other.m_buffer_manager, nullptr);
            m_texture_manager = std::exchange(other.m_texture_manager, nullptr);

            return *this;
        }

        //-------------------------------------------------------------------------
        void batch::append(const render_item& item, const glm::vec4& color, const glm::mat4& world)
        {
            m_buffer_manager->add_indices(item.get_component<index_component>());

            if (m_texture_manager->has_reserved_texture_space())
            {
                const vertex_component* vertex_comp = item.get_component<vertex_component>();
                const texture_component* texture_comp = item.get_component<texture_component>();

                assert(vertex_comp != nullptr);
                assert(texture_comp != nullptr);

                s32 sampler_id = m_texture_manager->add_texture(texture_comp->texture_id());
                if (sampler_id != -1)
                {
                    m_buffer_manager->add_vertices(vertex_comp, sampler_id, color, world);
                }
                else
                {
                    log::error("Unable to generate sampler id for texture.");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                m_buffer_manager->add_vertices(item.get_component<vertex_component>(), color, world);
            }
        }

        //-------------------------------------------------------------------------
        void batch::reset()
        {
            m_buffer_manager->reset();
            m_texture_manager->reset();
        }

        //-------------------------------------------------------------------------
        bool batch::can_add(s32 nr_vertices, s32 nr_indices) const
        {
            return m_buffer_manager->can_add(nr_vertices, nr_indices);
        }

        //-------------------------------------------------------------------------
        bool batch::has_reserved_texture_space() const { return m_texture_manager->has_reserved_texture_space(); }

        //-------------------------------------------------------------------------
        const void* batch::vertices() const { return m_buffer_manager->vertices(); }
        //-------------------------------------------------------------------------
        const void* batch::indices() const { return m_buffer_manager->indices(); }
        //-------------------------------------------------------------------------
        const s32* batch::samplers() const { return has_reserved_texture_space() ? m_texture_manager->samplers().data() : nullptr; }
        //-------------------------------------------------------------------------
        const u32* batch::textures() const { return has_reserved_texture_space() ? m_texture_manager->textures().data() : nullptr; }

        //-------------------------------------------------------------------------
        u32 batch::active_vertex_count() const { return m_buffer_manager->active_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 batch::active_index_count() const { return m_buffer_manager->active_index_count(); }
        //-------------------------------------------------------------------------
        u32 batch::active_sampler_count() const { return m_texture_manager->active_sampler_count(); }
        //-------------------------------------------------------------------------
        u32 batch::active_texture_count() const { return m_texture_manager->active_texture_count(); }

        //-------------------------------------------------------------------------
        u64 batch::vertex_buffer_byte_size() const { return m_buffer_manager->active_vertices_byte_size(); }
        //-------------------------------------------------------------------------
        u64 batch::index_buffer_byte_size() const { return m_buffer_manager->active_indices_byte_size(); }

        //-------------------------------------------------------------------------
        u32 batch::max_vertex_count() const { return m_buffer_manager->max_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 batch::max_index_count() const { return m_buffer_manager->max_index_count(); }
        //-------------------------------------------------------------------------
        u32 batch::max_texture_count() const { return m_texture_manager->max_texture_count(); }

        //-------------------------------------------------------------------------
        batch_drawing_data::batch_drawing_data(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count, s32 size_textures)
            : m_layouts(layouts)
            , m_layout_count(layout_count)
        {
            assert(size_vertex_buffer > 0);
            assert(size_index_buffer > 0);

            assert(layouts != nullptr);
            assert(layout_count > 0);

            // Already start with one batch
            m_batches.emplace_back(size_vertex_buffer, size_index_buffer, layouts, layout_count, size_textures);

            // Allocate VAO
            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);

            // Allocate VBO
            glGenBuffers(1, &m_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            const size_t size_vbo = internal::calculate_total_size_vertex_type(layouts, layout_count) * size_vertex_buffer;
            glBufferData(GL_ARRAY_BUFFER, size_vbo, nullptr, GL_DYNAMIC_DRAW);

            for (u64 i = 0; i < layout_count; ++i)
            {
                const vertex_attribute_layout& layout = layouts[i];

                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, layout.count, internal::convert_to_gl_data_type(layout.data_type), layout.normalized ? GL_TRUE : GL_FALSE, layout.stride, (void*)layout.offset);
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Allocate EBO
            glGenBuffers(1, &m_ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            const size_t size_ebo = sizeof(index) * size_index_buffer;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(0);
        }

        //-------------------------------------------------------------------------
        void batch_drawing_data::append(const render_item& item, const glm::vec4& color, const glm::mat4& world)
        {
            auto vertex_comp = item.get_component<vertex_component>();
            auto index_comp = item.get_component<index_component>();

            if (vertex_comp == nullptr || index_comp == nullptr)
            {
                log::error("render item does not have vertex or index component");
                exit(EXIT_FAILURE);
            }

            if (m_batches[m_push_batch].can_add(vertex_comp->vertex_count(), index_comp->index_count()))
            {
                m_batches[m_push_batch].append(item, color, world);
            }
            else
            {
                if (m_batches.size() <= m_push_batch + 1)
                {
                    u32 max_vertex_count = m_batches[m_push_batch].max_vertex_count();
                    u32 max_index_count = m_batches[m_push_batch].max_index_count();

                    // it might be that this batch does not support textures
                    s32 max_images = m_batches[m_push_batch].has_reserved_texture_space()
                        ? m_batches[m_push_batch].max_texture_count()
                        : -1;

                    m_batches.emplace_back(max_vertex_count, max_index_count, m_layouts, m_layout_count, max_images);
                }

                ++m_push_batch;

                append(item, color, world);
            }
        }

        //-------------------------------------------------------------------------
        void batch_drawing_data::reset()
        {
            for (batch& b : m_batches)
            {
                b.reset();
            }

            m_push_batch = 0;
            m_draw_batch = 0;
        }

        //-------------------------------------------------------------------------
        void batch_drawing_data::release()
        {
            reset();

            glDeleteBuffers(1, &m_vbo);
            glDeleteBuffers(1, &m_ebo);
            glDeleteVertexArrays(1, &m_vao);
        }

        //-------------------------------------------------------------------------
        void batch_drawing_data::load_first_batch()
        {
            m_draw_batch = 0;
        }

        //-------------------------------------------------------------------------
        const batch* batch_drawing_data::next_batch()
        {
            if (m_draw_batch < m_batches.size())
            {
                auto b = &m_batches[m_draw_batch];

                ++m_draw_batch;

                return b;
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        s32 batch_drawing_data::batch_count() const
        {
            return static_cast<s32>(m_batches.size());
        }

        //-------------------------------------------------------------------------
        u32 batch_drawing_data::vao() const { return m_vao; }
        //-------------------------------------------------------------------------
        u32 batch_drawing_data::vbo() const { return m_vbo; }
        //-------------------------------------------------------------------------
        u32 batch_drawing_data::ebo() const { return m_ebo; }
    }
}