#include "render/render_batch.h"
#include "render/render_vertex_buffer.h"
#include "render/render_index_buffer.h"
#include "render/render_storage_buffer.h"
#include "render/render_features.h"

#include "render/opengl/render_gl_api.h"

#include "render/helpers/render_vertex_buffer_ops.h"
#include "render/helpers/render_index_buffer_ops.h"
#include "render/helpers/render_storage_buffer_ops.h"

#include "resources/material_pool.h"

#include "util/types.h"
#include "util/log.h"
#include "util/pointer_math.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static void check_drawing_type(u32 index_count, GLenum type)
            {
                if (index_count == 0)
                {
                    return;
                }

                switch (type)
                {
                case GL_LINES:
                    if (index_count % 2 != 0)
                    {
                        log::error("Trying to render invalid number of lines: {}", index_count);
                        return;
                    }
                    break;
                case GL_TRIANGLES:
                    if (index_count % 3 != 0)
                    {
                        log::error("Trying to render invalid number of triangles: {}", index_count);
                        return;
                    }
                    break;
                }
            }
            //-------------------------------------------------------------------------
            static u32 topology(topology_type type)
            {
                switch (type)
                {
                case topology_type::POINTS: return GL_POINTS;
                case topology_type::LINES: return GL_LINES;
                case topology_type::TRIANGLES: return GL_TRIANGLES;
                }

                log::error("Invalid topology_type type specified, using GL_TRIANGLES");
                return GL_TRIANGLES;
            }
            //-------------------------------------------------------------------------
            static u32 index_type()
            {
                if (sizeof(index) == sizeof(u32)) return GL_UNSIGNED_INT;
                if (sizeof(index) == sizeof(u16)) return GL_UNSIGNED_SHORT;

                log::error("Invalid index type specified: {}, using UNSIGNED_INT", typeid(index).name());
                return GL_UNSIGNED_INT;
            }
        }

        //-------------------------------------------------------------------------
        // Buffer Manager
        class batch_buffer_manager
        {
        public:
            //-------------------------------------------------------------------------
            batch_buffer_manager(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count)
                : m_max_vertex_count(size_vertex_buffer)
                , m_max_index_count(size_index_buffer)
                , m_vertex_buffer(size_vertex_buffer, layouts, layout_count)
                , m_index_buffer(size_index_buffer)
            {
                assert(size_vertex_buffer > 0);
                assert(size_index_buffer > 0);

                assert(layouts != nullptr);
                assert(layout_count > 0);
            }

            //-------------------------------------------------------------------------
            bool can_add(s32 nr_vertices, s32 nr_indices) const
            {
                return m_vertex_buffer.can_add(nr_vertices) && m_index_buffer.can_add(nr_indices);
            }
            //-------------------------------------------------------------------------
            bool has_data() const
            {
                return m_vertex_buffer.active_vertex_count() > 0 || m_index_buffer.active_index_count() > 0;
            }

            //-------------------------------------------------------------------------
            void add_vertices(const irender_item* item, s32 material_id, const glm::vec4& color, const glm::mat4& world)
            {
                u64 start_index = m_vertex_buffer.active_vertex_count();
                u64 end_index = start_index + item->vertex_count();

                copy_vertex_data(item, material_id, color);
                transform_vertex_positions(start_index, end_index, world);

                if (m_vertex_buffer.has_layout(attribute_type::NORMAL))
                {
                    transform_vertex_normals(start_index, end_index, world);
                }
            }
            //-------------------------------------------------------------------------
            void add_indices(const irender_item* item)
            {
                assert(!item->faces().empty());

                auto index_buffer = &m_index_buffer;
                u64 start_index = index_buffer->active_index_count();
                u64 end_index = start_index + item->index_count();

                copy_index_data(item);
                transform_index_locations(start_index, end_index, m_vertex_buffer.active_vertex_count());
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                m_vertex_buffer.submit();

                if (active_index_count() != 0)
                {
                    m_index_buffer.submit();
                }
            }

            //-------------------------------------------------------------------------
            void reset()
            {
                m_vertex_buffer.reset();
                m_index_buffer.reset();
            }
            //-------------------------------------------------------------------------
            void release()
            {
                m_vertex_buffer.free();
                m_index_buffer.free();
            }

            //-------------------------------------------------------------------------
            u64 vertex_size_in_bytes() const { return m_vertex_buffer.vertex_size_in_bytes(); }
            u64 index_size_in_bytes() const { return m_index_buffer.index_size_in_bytes(); }

            //-------------------------------------------------------------------------
            u32 active_vertex_count() const { return m_vertex_buffer.active_vertex_count(); }
            u64 active_vertices_byte_size() const { return m_vertex_buffer.total_size_in_bytes(); }
            u32 active_index_count() const { return m_index_buffer.active_index_count(); }
            u64 active_indices_byte_size() const { return m_index_buffer.total_size_in_bytes(); }

            //-------------------------------------------------------------------------
            u32 max_vertex_count() const { return m_max_vertex_count; }
            u32 max_index_count() const { return m_max_index_count; }

            //-------------------------------------------------------------------------
            const void* vertices() const { return m_vertex_buffer.data(); }
            const void* indices() const { return m_index_buffer.data(); }

        private:
            //-------------------------------------------------------------------------
            void copy_vertex_data(const irender_item* item, s32 material_id, const glm::vec4& color)
            {
                vertex_buffer_ops::vertex_attribute_addition_scope vaas(m_vertex_buffer, item->vertex_count());

                if (m_vertex_buffer.has_layout(attribute_type::POSITION))
                {
                    assert(item->vertex_positions().empty() == false);

                    vertex_buffer_ops::set_attribute_data(vaas, attribute_type::POSITION, item->vertex_positions().data());
                }
                
                if (m_vertex_buffer.has_layout(attribute_type::NORMAL))
                {
                    if (item->vertex_normals().empty() == false)
                    {
                        vertex_buffer_ops::set_attribute_data(vaas, attribute_type::NORMAL, item->vertex_normals().data());
                    }
                    else
                    {
                        glm::vec3 zero_normal(0.0f, 0.0f, 0.0f);
                        vertex_buffer_ops::set_attribute_data(vaas, attribute_type::NORMAL, glm::value_ptr(zero_normal));
                    }
                }
                if (m_vertex_buffer.has_layout(attribute_type::TEXCOORD))
                {
                    if (item->vertex_uvs().empty() == false)
                    {
                        vertex_buffer_ops::set_attribute_data(vaas, attribute_type::TEXCOORD, item->vertex_uvs().data());
                    }
                    else
                    {
                        glm::vec2 zero_uv(0.0f, 0.0f);
                        vertex_buffer_ops::set_attribute_data(vaas, attribute_type::TEXCOORD, glm::value_ptr(zero_uv));
                    }
                }

                if (m_vertex_buffer.has_layout(attribute_type::MATERIAL_INDEX) && material_id != -1)
                {
                    vertex_buffer_ops::map_attribute_data(vaas, attribute_type::MATERIAL_INDEX, (void*)&material_id);
                }

                vertex_buffer_ops::map_attribute_data(vaas, attribute_type::COLOR, (void*)&color[0]);
            }
            //-------------------------------------------------------------------------
            void copy_index_data(const irender_item* item)
            {
                index_buffer_ops::index_addition_scope ias(m_index_buffer, item->index_count());

                assert(sizeof(item->faces()[0][0]) == sizeof(index) && "different index size was used");

                index_buffer_ops::set_index_data(ias, item->faces().data());
            }

            //-------------------------------------------------------------------------
            void transform_vertex_positions(u64 start_index, u64 end_index, const glm::mat4& world)
            {
                vertex_buffer_ops::transform_attribute_data<glm::vec3>(m_vertex_buffer, attribute_type::POSITION, start_index, end_index, [&](glm::vec3& position)
                {
                    glm::vec4 transformed_pos = world * glm::vec4(position, 1.0f);

                    position.x = transformed_pos.x;
                    position.y = transformed_pos.y;
                    position.z = transformed_pos.z;
                });
            }
            //-------------------------------------------------------------------------
            void transform_vertex_normals(u64 start_index, u64 end_index, const glm::mat4& world)
            {
                glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(world)));

                vertex_buffer_ops::transform_attribute_data<glm::vec3>(m_vertex_buffer, attribute_type::NORMAL, start_index, end_index, [&](glm::vec3& normal)
                {
                    glm::vec3 transformed_normal = normal_matrix * normal;

                    normal = glm::normalize(transformed_normal);
                });
            }
            //-------------------------------------------------------------------------
            void transform_vertex_diffuse_texture_ids(u64 start_index, u64 end_index, s32 sampler_id)
            {
                vertex_buffer_ops::transform_attribute_data<s32>(m_vertex_buffer, attribute_type::MATERIAL_INDEX, start_index, end_index, [&](s32& id)
                {
                    id = sampler_id;
                });
            }           
            //-------------------------------------------------------------------------
            void transform_index_locations(u64 start_index, u64 end_index, u64 offset)
            {
                index_buffer_ops::transform_index_data(m_index_buffer, start_index, end_index, 
                    [&](index& index)
                {
                    index += offset;
                });
            }

            const s32 m_max_vertex_count = 0;
            const s32 m_max_index_count = 0;

            vertex_buffer m_vertex_buffer;
            index_buffer m_index_buffer;
        };

        //-------------------------------------------------------------------------
        // Material Buffer Manager
        namespace batch_material_storage
        {
            //-------------------------------------------------------------------------
            // Unaligned size in bytes of the structure storage on the GPU
            static u64 size_in_bytes()
            {
                u64 total_size_in_bytes = sizeof(s32) * max_textures()  // max amount of textures that can be bound at once
                                        + sizeof(s32)                   // actual size of textures that are bound
                                        + sizeof(glm::vec4)             // ambient color 
                                        + sizeof(glm::vec4);            // diffuse color

                return total_size_in_bytes;
            }
        };

        class batch_material_manager
        {
        public:
            //-------------------------------------------------------------------------
            batch_material_manager()
                :m_storage_buffer(8, batch_material_storage::size_in_bytes())
            {}

            //-------------------------------------------------------------------------
            bool can_add(s32 nr_elements) const
            {
                return m_storage_buffer.can_add(nr_elements);
            }

            //-------------------------------------------------------------------------
            s32 add_material_attributes(const irender_item* item)
            {
                s32 material_index = m_storage_buffer.active_element_count();

                copy_material_data(item);

                return material_index;
            }

            //-------------------------------------------------------------------------
            void bind()
            {
                m_storage_buffer.bind(1);
            }

            //-------------------------------------------------------------------------
            void unbind()
            {
                m_storage_buffer.unbind();
            }

            //-------------------------------------------------------------------------
            void submit()
            {
                m_storage_buffer.submit(1);
            }

            //-------------------------------------------------------------------------
            void reset()
            {
                m_storage_buffer.reset();
            }

            //-------------------------------------------------------------------------
            void release()
            {
                m_storage_buffer.free();
            }

        public:
            //-------------------------------------------------------------------------
            u64 element_size_in_bytes() const { return m_storage_buffer.element_size_in_bytes(); }

            //-------------------------------------------------------------------------
            u32 active_element_count() const { return m_storage_buffer.active_element_count(); }
            u64 active_elements_byte_size() const { return m_storage_buffer.total_size_in_bytes(); }

        private:
            void copy_material_data(const irender_item* item)
            {
                const resources::imaterial* material = item->material();
                if (material == nullptr)
                {
                    return;
                }

                storage_buffer_ops::storage_data_addition_scope sdas(m_storage_buffer, 1);

                std::vector<u8> material_data(m_storage_buffer.element_size_in_bytes());

                size_t offset = 0;

                offset = copy_texture_samplers(material, material_data.data(), offset);
                offset = copy_material_properties(material, material_data.data(), offset);

                storage_buffer_ops::set_storage_data(sdas, material_data.data());
            }

            //-------------------------------------------------------------------------
            u64 copy_texture_samplers(const resources::imaterial* material, u8* buffer, u64 offset)
            {
                // For more info on why alignment is 4 see:
                // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
                const s32 alignment = 4;
                const s32 sampler_count = static_cast<s32>(material->samplers().size());

                assert(sampler_count <= max_textures());

                offset = memory::align_up(offset, alignment); // Align for `int`.

                if (material->has_textures())
                {
                    const s32* samplers = material->samplers().data();
                    std::memcpy(buffer + offset, samplers, sampler_count * sizeof(s32));
                    offset += sampler_count * sizeof(s32);
                }

                // Pad remaining samplers with -1
                const s32 padding_value = -1;
                const u64 padding_size = (max_textures() - sampler_count);

                if (padding_size > 0) 
                {
                    std::memset(buffer + offset, padding_value, padding_size * sizeof(s32));
                    offset += padding_size * sizeof(s32);
                }

                // Store sampler count
                offset = memory::align_up(offset, alignment); // Align for `int`.
                std::memcpy(buffer + offset, &sampler_count, sizeof(sampler_count));
                offset += sizeof(s32);

                return offset;
            }

            //-------------------------------------------------------------------------
            u64 copy_material_properties(const resources::imaterial* material, u8* buffer, u64 offset)
            {
                // For more info on why alignment is 16 see:
                // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
                const s32 alignment = 16;

                const glm::vec4& ambient_color = material->ambient_color();
                const glm::vec4& diffuse_color = material->diffuse_color();

                offset = memory::align_up(offset, alignment); // Align for `vec4`.
                std::memcpy(buffer + offset, &ambient_color, sizeof(glm::vec4));
                offset += sizeof(glm::vec4);

                offset = memory::align_up(offset, alignment); // Align for `vec4`.
                std::memcpy(buffer + offset, &diffuse_color, sizeof(glm::vec4));
                offset += sizeof(glm::vec4);

                return offset;
            }

        private:
            storage_buffer m_storage_buffer;
        };

        //-------------------------------------------------------------------------
        // Batch Impl
        class batch::impl
        {
        public:
            //-------------------------------------------------------------------------
            impl(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count)
                :m_buffer_manager(nullptr)
                ,m_vao(0)
            {
                assert(layouts != nullptr);
                assert(layout_count > 0);

                // Allocate VAO
                opengl::api::instance().generate_vertex_arrays(1, &m_vao);
                opengl::api::instance().bind_vertex_array(m_vao);

                m_buffer_manager = std::make_unique<batch_buffer_manager>(size_vertex_buffer, size_index_buffer, layouts, layout_count);
                m_material_manager = std::make_unique<batch_material_manager>();

                opengl::api::instance().bind_vertex_array(0);
            }

            //-------------------------------------------------------------------------
            ~impl()
            {
                if (m_vao)
                {
                    opengl::api::instance().delete_vertex_arrays(1, &m_vao);
                }
            }

            
            //-------------------------------------------------------------------------
            void bind() const
            {
                opengl::api::instance().bind_vertex_array(m_vao);

                m_material_manager->bind();
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                m_material_manager->unbind();

                opengl::api::instance().bind_vertex_array(0);
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                m_buffer_manager->submit();
                m_material_manager->submit();
            }

            //-------------------------------------------------------------------------
            void release()
            {
                opengl::api::instance().delete_vertex_arrays(1, &m_vao);

                m_vao = 0;
            }

            //-------------------------------------------------------------------------
            void draw(topology_type topology) const
            {
                GLenum gl_topology = internal::topology(topology);

#ifndef NDEBUG
                internal::check_drawing_type(m_buffer_manager->active_index_count(), gl_topology);
#endif
                if (m_buffer_manager->active_index_count() != 0)
                {
                    opengl::api::instance().draw_elements(gl_topology, m_buffer_manager->active_index_count(), internal::index_type(), nullptr);
                }
                else
                {
                    opengl::api::instance().draw_arrays(gl_topology, 0, m_buffer_manager->active_vertex_count());
                }
            }

            std::unique_ptr<batch_buffer_manager> m_buffer_manager;
            std::unique_ptr<batch_material_manager> m_material_manager;

            u32 m_vao;
        };

        //-------------------------------------------------------------------------
        // Batch
        batch::batch(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count)
            : m_pimpl(std::make_unique<impl>(size_vertex_buffer, size_index_buffer, layouts, layout_count))
        {}
        //-------------------------------------------------------------------------
        batch::~batch() = default;
        //-------------------------------------------------------------------------
        batch::batch(batch&& other) noexcept = default;
        //-------------------------------------------------------------------------
        batch& batch::operator=(batch&& other) noexcept = default;

        //-------------------------------------------------------------------------
        void batch::bind() const
        {
            m_pimpl->bind();
        }
        //-------------------------------------------------------------------------
        void batch::unbind() const
        {
            m_pimpl->unbind();
        }
        //-------------------------------------------------------------------------
        void batch::submit() const
        {
            m_pimpl->submit();
        }
        //-------------------------------------------------------------------------
        void batch::draw(topology_type topology) const
        {
            m_pimpl->draw(topology);
        }

        //-------------------------------------------------------------------------
        void batch::append(const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {
            s32 material_id = m_pimpl->m_material_manager->add_material_attributes(item);

            if (item->index_count() != 0)
            {
                m_pimpl->m_buffer_manager->add_indices(item);
            }

            m_pimpl->m_buffer_manager->add_vertices(item, material_id, color, world);
        }
        //-------------------------------------------------------------------------
        void batch::reset()
        {
            m_pimpl->m_buffer_manager->reset();
            m_pimpl->m_material_manager->reset();
        }
        //-------------------------------------------------------------------------
        void batch::release()
        {
            m_pimpl->m_buffer_manager->release();
            m_pimpl->m_material_manager->release();
            m_pimpl->release();
        }

        //-------------------------------------------------------------------------
        bool batch::can_add(s32 nr_vertices, s32 nr_indices) const
        {
            return m_pimpl->m_buffer_manager->can_add(nr_vertices, nr_indices)
                && m_pimpl->m_material_manager->can_add(1);
        }
        //-------------------------------------------------------------------------
        bool batch::has_data() const
        {
            return m_pimpl->m_buffer_manager->has_data();
        }

        //-------------------------------------------------------------------------
        const void* batch::vertices() const { return m_pimpl->m_buffer_manager->vertices(); }
        //-------------------------------------------------------------------------
        const void* batch::indices() const { return m_pimpl->m_buffer_manager->indices(); }

        //-------------------------------------------------------------------------
        u32 batch::active_vertex_count() const { return m_pimpl->m_buffer_manager->active_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 batch::active_index_count() const { return m_pimpl->m_buffer_manager->active_index_count(); }

        //-------------------------------------------------------------------------
        u64 batch::vertex_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_vertices_byte_size(); }
        //-------------------------------------------------------------------------
        u64 batch::index_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_indices_byte_size(); }

        //-------------------------------------------------------------------------
        u32 batch::max_vertex_count() const { return m_pimpl->m_buffer_manager->max_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 batch::max_index_count() const { return m_pimpl->m_buffer_manager->max_index_count(); }

        //-------------------------------------------------------------------------
        // Batch Drawing Data Impl
        struct batch_drawing_data::impl
        {
            //-------------------------------------------------------------------------
            impl(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count)
                : layouts(layouts)
                , layout_count(layout_count)
            {
                assert(size_vertex_buffer > 0);
                assert(size_index_buffer > 0);

                assert(layouts != nullptr);
                assert(layout_count > 0);

                // Already start with one batch
                batches.emplace_back(size_vertex_buffer, size_index_buffer, layouts, layout_count);
            }

            s32                         draw_batch      = 0;
            s32                         push_batch      = 0;

            batch_arr                   batches         = {};

            const attribute_layout*     layouts         = nullptr;
            const u64                   layout_count    = 0;
        };

        //-------------------------------------------------------------------------
        batch_drawing_data::batch_drawing_data(s32 size_vertex_buffer, s32 size_index_buffer, const attribute_layout* layouts, u64 layout_count)
            : m_pimpl(std::make_unique<impl>(size_vertex_buffer, size_index_buffer, layouts, layout_count))
        {
        }

        //-------------------------------------------------------------------------
        batch_drawing_data::~batch_drawing_data() = default;
        //-------------------------------------------------------------------------
        batch_drawing_data::batch_drawing_data(batch_drawing_data&& other) noexcept = default;
        //-------------------------------------------------------------------------
        batch_drawing_data& batch_drawing_data::operator=(batch_drawing_data && other) noexcept = default;

        //-------------------------------------------------------------------------
        void batch_drawing_data::append(const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {           
            if (item->vertex_count() == 0)
            {
                log::error("render item does not have vertices");
                exit(EXIT_FAILURE);
            }
            
            if (m_pimpl->batches[m_pimpl->push_batch].can_add(item->vertex_count(), item->index_count()))
            {
                m_pimpl->batches[m_pimpl->push_batch].append(item, color, world);
            }
            else
            {
                m_pimpl->batches[m_pimpl->push_batch].bind();
                m_pimpl->batches[m_pimpl->push_batch].submit();

                if (m_pimpl->batches.size() <= m_pimpl->push_batch + 1)
                {
                    u32 max_vertex_count = m_pimpl->batches[m_pimpl->push_batch].max_vertex_count();
                    u32 max_index_count = m_pimpl->batches[m_pimpl->push_batch].max_index_count();
            
                    m_pimpl->batches.emplace_back(max_vertex_count, max_index_count, m_pimpl->layouts, m_pimpl->layout_count);
                }
            
                ++m_pimpl->push_batch;
            
                append(item, color, world);
            }
        }       
        //-------------------------------------------------------------------------
        void batch_drawing_data::reset()
        {
            // We clear everything if we are in immediate mode
            for (batch& b : m_pimpl->batches)
            {
                b.reset();
            }

            m_pimpl->push_batch = 0;
            m_pimpl->draw_batch = 0;
        }        
        //-------------------------------------------------------------------------
        void batch_drawing_data::release()
        {
            for (batch& b : m_pimpl->batches)
            {
                b.reset();
                b.release();
            }

            m_pimpl->push_batch = 0;
            m_pimpl->draw_batch = 0;
        }

        //-------------------------------------------------------------------------
        const batch* batch_drawing_data::first_batch()
        {
            m_pimpl->draw_batch = 0;

            return next_batch();
        }
        //-------------------------------------------------------------------------
        const batch* batch_drawing_data::next_batch()
        {
            if (m_pimpl->draw_batch < m_pimpl->batches.size())
            {
                auto b = &m_pimpl->batches[m_pimpl->draw_batch];

                ++m_pimpl->draw_batch;

                return b;
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        bool batch_drawing_data::has_drawing_data() const
        {
            return std::any_of(m_pimpl->batches.cbegin(), m_pimpl->batches.cend(), [](const batch& b) { return b.has_data(); });
        }
    }
}