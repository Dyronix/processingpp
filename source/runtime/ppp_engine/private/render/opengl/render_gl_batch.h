#pragma once

#include "util/types.h"
#include "util/log.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <unordered_map>

namespace ppp
{
    namespace render
    {
        template<typename T>
        class PrimitiveBatch
        {
        public:
            PrimitiveBatch(s32 size_vertex_buffer, s32 size_index_buffer)
                : m_max_vertex_count(size_vertex_buffer)
                , m_max_index_count(size_index_buffer)
            {
                assert(size_vertex_buffer > 0);
                assert(size_index_buffer > 0);

                m_vertices = std::make_unique<T[]>(size_vertex_buffer);
                m_indices = std::make_unique<Index[]>(size_index_buffer);
            }

            void append(const render_item& item, const glm::vec4& color, const glm::mat4& world)
            {
                auto vertex_comp = item.get_component<vertex_component>();
                auto index_comp = item.get_component<index_component>();

                assert(vertex_comp != nullptr);
                assert(index_comp != nullptr);

                u64 vert_count = vertex_comp->vertex_count();
                auto vert_positions = vertex_comp->get_attribute_data<glm::vec3>(vertex_attribute_type::POSITION);
                auto idxs = index_comp->indices();
                u64 idx_count = index_comp->index_count();

                memcpy(&m_indices[m_nr_active_indices], idxs, sizeof(Index) * idx_count);

                // For each index that was added we need to offset it with the amount of indices that are already within the array.
                for (s32 i = 0; i < idx_count; ++i)
                {
                    m_indices[m_nr_active_indices + i] += m_nr_active_vertices;
                }

                m_nr_active_indices += idx_count;

                T fmt = {};

                for (s32 i = 0; i < vert_count; ++i)
                {
                    glm::vec4 transformed_position = world * glm::vec4(vert_positions[i], 1.0f);

                    fmt.position.x = transformed_position.x;
                    fmt.position.y = transformed_position.y;
                    fmt.position.z = transformed_position.z;
                    fmt.color = color;

                    m_vertices[m_nr_active_vertices] = fmt;
                    ++m_nr_active_vertices;
                }

                ++m_nr_primitives;
            }

            void reset()
            {
                m_nr_active_vertices = 0;
                m_nr_active_indices = 0;
                m_nr_primitives = 0;
            }

            bool can_add(s32 nr_vertices, s32 nr_indices) const
            {
                return m_nr_active_vertices + nr_vertices < m_max_vertex_count && m_nr_active_indices + nr_indices < m_max_index_count;
            }

            const T* vertices() const { return m_vertices.get(); }
            const Index* indices() const { return m_indices.get(); }

            u32 active_vertex_count() const { return m_nr_active_vertices; }
            u32 active_index_count() const { return m_nr_active_indices; }
            u32 active_primitive_count() const { return m_nr_primitives; }

            u64 vertex_buffer_byte_size() const { return sizeof(T) * m_nr_active_vertices; }
            u64 index_buffer_byte_size() const { return sizeof(Index) * m_nr_active_indices; }

            u32 max_vertex_count() const { return m_max_vertex_count; }
            u32 max_index_count() const { return m_max_index_count; }

        private:
            using VertexBuffer = std::unique_ptr<T[]>;
            using IndexBuffer = std::unique_ptr<Index[]>;

            VertexBuffer    m_vertices = nullptr;
            IndexBuffer     m_indices = nullptr;

            u32             m_nr_active_vertices = 0;
            u32             m_nr_active_indices = 0;
            u32             m_nr_primitives = 0;

            const u32       m_max_vertex_count = 0;
            const u32       m_max_index_count = 0;
        };
        template<typename T>
        class PrimitiveDrawingData
        {
        public:
            PrimitiveDrawingData(s32 size_vertex_buffer, s32 size_index_buffer)
            {
                assert(size_vertex_buffer > 0);
                assert(size_index_buffer > 0);

                // Already start with one batch
                m_batches.push_back(PrimitiveBatch<T>(size_vertex_buffer, size_index_buffer));

                // Allocate VAO
                glGenVertexArrays(1, &m_vao);
                glBindVertexArray(m_vao);

                // Allocate VBO
                glGenBuffers(1, &m_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                const size_t size_vbo = sizeof(T) * size_vertex_buffer;
                glBufferData(GL_ARRAY_BUFFER, size_vbo, nullptr, GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                auto pos_offset = offsetof(T, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), reinterpret_cast<void*>(pos_offset));

                glEnableVertexAttribArray(1);
                auto col_offset = offsetof(T, color);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(T), reinterpret_cast<void*>(col_offset));

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Allocate EBO
                glGenBuffers(1, &m_ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
                const size_t size_ebo = sizeof(Index) * size_index_buffer;
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(0);
            }

            void append(const render_item& item, const glm::vec4& color, const glm::mat4& world)
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

                        m_batches.push_back(PrimitiveBatch<T>(max_vertex_count, max_index_count));
                    }

                    ++m_push_batch;

                    append(item, color, world);
                }
            }

            void reset()
            {
                for (PrimitiveBatch<T>& b : m_batches)
                {
                    b.reset();
                }

                m_push_batch = 0;
                m_draw_batch = 0;
            }

            void release()
            {
                reset();

                glDeleteBuffers(1, &m_vbo);
                glDeleteBuffers(1, &m_ebo);
                glDeleteVertexArrays(1, &m_vao);
            }

            void load_first_batch()
            {
                m_draw_batch = 0;
            }

            const PrimitiveBatch<T>* next_batch()
            {
                if (m_draw_batch < m_batches.size())
                {
                    auto b = &m_batches[m_draw_batch];

                    ++m_draw_batch;

                    return b;
                }

                return nullptr;
            }

            s32 batch_count() const
            {
                return static_cast<s32>(m_batches.size());
            }

            u32 vao() const { return m_vao; }
            u32 vbo() const { return m_vbo; }
            u32 ebo() const { return m_ebo; }

        private:
            using BatchArr = std::vector<PrimitiveBatch<T>>;

            u32	m_vao = 0;
            u32	m_vbo = 0;
            u32 m_ebo = 0;

            s32 m_draw_batch = 0;
            s32 m_push_batch = 0;

            BatchArr m_batches;
        };

        template<typename T>
        class TextureBatch
        {
        public:
            using ImageMap = std::unordered_map<u32, s32>;

            TextureBatch(s32 size_textures)
                : m_max_texture_count(size_textures)
            {
                assert(size_textures > 0);

                m_size_vertex_buffer = size_textures * 4;
                m_size_index_buffer = size_textures * 4 * 3;

                m_vertices = std::make_unique<T[]>(m_size_vertex_buffer);
                m_indices = std::make_unique<Index[]>(m_size_index_buffer);

                m_image_sampler_ids = std::make_unique<s32[]>(size_textures);
                m_image_ids.reserve(size_textures);
            }

            void append(const render_item& item, const glm::vec4& color, const glm::mat4& world)
            {
                auto vertex_comp = item.get_component<vertex_component>();
                auto index_comp = item.get_component<index_component>();

                assert(vertex_comp != nullptr);
                assert(index_comp != nullptr);

                auto texture_comp = item.get_component<texture_component>();
                if (texture_comp == nullptr)
                {
                    log::error("render item does not have texture component");
                    exit(EXIT_FAILURE);
                }

                u64 vert_count = vertex_comp->vertex_count();
                auto vert_positions = vertex_comp->get_attribute_data<glm::vec3>(vertex_attribute_type::POSITION);
                auto vert_texcoords = vertex_comp->get_attribute_data<glm::vec2>(vertex_attribute_type::TEXCOORD);
                auto idxs = index_comp->indices();
                u64 idx_count = index_comp->index_count();
                u32 image_id = texture_comp->texture_id();

                bool existing_image = m_image_ids.find(image_id) != std::cend(m_image_ids);

                memcpy(&m_indices[m_nr_active_indices], idxs, sizeof(Index) * idx_count);
                // For each index that was added we need to offset it with the amount of indices that are already within the array.
                for (s32 i = 0; i < idx_count; ++i)
                {
                    m_indices[m_nr_active_indices + i] += m_nr_active_vertices;
                }
                m_nr_active_indices += idx_count;

                T fmt = {};

                for (s32 i = 0; i < vert_count; ++i)
                {
                    glm::vec4 transformed_position = world * glm::vec4(vert_positions[i], 1.0f);
                    fmt.position.x = transformed_position.x;
                    fmt.position.y = transformed_position.y;
                    fmt.position.z = transformed_position.z;
                    fmt.texcoord = vert_texcoords[i];
                    fmt.color = color;
                    fmt.texture_idx = existing_image ? (f32)m_image_ids.at(image_id) : (f32)m_nr_triangles;
                    m_vertices[m_nr_active_vertices] = fmt;
                    ++m_nr_active_vertices;
                }

                if (!existing_image)
                {
                    m_image_sampler_ids[m_nr_triangles] = m_nr_triangles;
                    m_image_ids.insert(std::make_pair(image_id, m_nr_triangles));

                    ++m_nr_triangles;
                }
            }

            void reset()
            {
                m_nr_active_vertices = 0;
                m_nr_active_indices = 0;
                m_nr_triangles = 0;

                m_image_ids.clear();
            }

            bool can_add(s32 nr_vertices, s32 nr_indices) const
            {
                return m_nr_active_vertices + nr_vertices < m_size_vertex_buffer && m_nr_active_indices + nr_indices < m_size_index_buffer;
            }

            const T* vertices() const { return m_vertices.get(); }
            const Index* indices() const { return m_indices.get(); }
            const s32* samplers() const { return m_image_sampler_ids.get(); }
            const ImageMap& image_ids() const { return m_image_ids; }

            u32 active_vertex_count() const { return m_nr_active_vertices; }
            u32 active_index_count() const { return m_nr_active_indices; }
            u32 active_texture_count() const { return m_nr_triangles; }

            u64 vertex_buffer_byte_size() const { return sizeof(T) * m_nr_active_vertices; }
            u64 index_buffer_byte_size() const { return sizeof(Index) * m_nr_active_indices; }

            u32 max_texture_count() const { return m_max_texture_count; }

        private:
            using VertexBuffer = std::unique_ptr<T[]>;
            using IndexBuffer = std::unique_ptr<Index[]>;
            using SamplerBuffer = std::unique_ptr<s32[]>;

            VertexBuffer    m_vertices = nullptr;
            IndexBuffer     m_indices = nullptr;

            u32             m_nr_active_vertices = 0;
            u32             m_nr_active_indices = 0;
            u32             m_nr_triangles = 0;

            SamplerBuffer   m_image_sampler_ids = {};
            ImageMap        m_image_ids = {};

            const u32       m_max_texture_count = 0;

            s32             m_size_vertex_buffer = 0;
            s32             m_size_index_buffer = 0;
        };
        template<typename T>
        class TextureDrawingData
        {
        public:
            TextureDrawingData(s32 size_textures)
            {
                assert(size_textures > 0);

                // Already start with one batch
                m_batches.push_back(TextureBatch<T>(size_textures));

                s32 size_vertex_buffer = size_textures * 4;
                s32 size_index_buffer = size_textures * 4 * 3;

                // Allocate VAO
                glGenVertexArrays(1, &m_vao);
                glBindVertexArray(m_vao);

                // Allocate VBO
                glGenBuffers(1, &m_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                const size_t size_vbo = sizeof(T) * size_vertex_buffer;
                glBufferData(GL_ARRAY_BUFFER, size_vbo, nullptr, GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                auto pos_offset = offsetof(T, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), reinterpret_cast<void*>(pos_offset));

                glEnableVertexAttribArray(1);
                auto tc_offset = offsetof(T, texcoord);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(T), reinterpret_cast<void*>(tc_offset));

                glEnableVertexAttribArray(2);
                auto col_offset = offsetof(T, color);
                glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(T), reinterpret_cast<void*>(col_offset));

                glEnableVertexAttribArray(3);
                auto ti_offset = offsetof(T, texture_idx);
                glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(T), reinterpret_cast<void*>(ti_offset));

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Allocate EBO
                glGenBuffers(1, &m_ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
                const size_t size_ebo = sizeof(Index) * size_index_buffer;
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(0);
            }

            void append(const render_item& item, const glm::vec4& color, const glm::mat4& world)
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
                        u32 max_texture_count = m_batches[m_push_batch].max_texture_count();

                        m_batches.push_back(TextureBatch<T>(max_texture_count));
                    }

                    ++m_push_batch;

                    append(item, color, world);
                }
            }

            void reset()
            {
                for (TextureBatch<T>& b : m_batches)
                {
                    b.reset();
                }

                m_push_batch = 0;
                m_draw_batch = 0;
            }

            void release()
            {
                reset();

                glDeleteBuffers(1, &m_vbo);
                glDeleteBuffers(1, &m_ebo);
                glDeleteVertexArrays(1, &m_vao);
            }

            void load_first_batch()
            {
                m_draw_batch = 0;
            }

            const TextureBatch<T>* next_batch()
            {
                if (m_draw_batch < m_batches.size())
                {
                    auto b = &m_batches[m_draw_batch];

                    ++m_draw_batch;

                    return b;
                }

                return nullptr;
            }

            s32 batch_count() const
            {
                return static_cast<s32>(m_batches.size());
            }

            u32 vao() const { return m_vao; }
            u32 vbo() const { return m_vbo; }
            u32 ebo() const { return m_ebo; }

        private:
            using BatchArr = std::vector<TextureBatch<T>>;

            u32	m_vao = 0;
            u32	m_vbo = 0;
            u32 m_ebo = 0;

            s32 m_draw_batch = 0;
            s32 m_push_batch = 0;

            BatchArr m_batches;
        };
    }
}