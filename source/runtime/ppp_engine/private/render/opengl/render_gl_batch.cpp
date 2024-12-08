#include "render/render_batch.h"
#include "render/render_vertex_buffer.h"
#include "render/render_index_buffer.h"
#include "render/render_shaders.h"

#include "render/opengl/render_gl_error.h"

#include "render/helpers/render_vertex_buffer_ops.h"
#include "render/helpers/render_index_buffer_ops.h"

#include "resources/texture_registry.h"

#include "util/types.h"
#include "util/log.h"

#include <glad/glad.h>

#include <numeric>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static void check_drawing_type(u32 index_count, GLenum type)
            {
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

        class buffer_manager
        {
        public:
            //-------------------------------------------------------------------------
            buffer_manager(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count)
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

            //-------------------------------------------------------------------------
            bool can_add(s32 nr_vertices, s32 nr_indices) const
            {
                return m_vertex_buffer.active_vertex_count() + nr_vertices < m_max_vertex_count && m_index_buffer.active_index_count() + nr_indices < m_max_index_count;
            }

            //-------------------------------------------------------------------------
            bool has_data() const
            {
                return m_vertex_buffer.active_vertex_count() > 0 || m_index_buffer.active_index_count() > 0;
            }

            //-------------------------------------------------------------------------
            void add_vertices(const irender_item* item, const glm::vec4& color, const glm::mat4& world)
            {
                s32 start_index = m_vertex_buffer.active_vertex_count();
                s32 end_index = start_index + item->vertex_count();

                copy_vertex_data(item, color);

                transform_vertex_positions(start_index, end_index, world);
                if (m_vertex_buffer.has_layout(vertex_attribute_type::NORMAL))
                {
                    transform_vertex_normals(start_index, end_index, world);
                }
            }

            //-------------------------------------------------------------------------
            void add_vertices(const irender_item* item, s32 sampler_id, const glm::vec4& color, const glm::mat4& world)
            {
                s32 start_index = m_vertex_buffer.active_vertex_count();
                s32 end_index = start_index + item->vertex_count();

                add_vertices(item, color, world);

                transform_vertex_diffuse_texture_ids(start_index, end_index, sampler_id);
            }

            //-------------------------------------------------------------------------
            void add_indices(const irender_item* item)
            {
                assert(!item->faces().empty());

                s32 start_index = m_index_buffer.active_index_count();
                s32 end_index = start_index + item->index_count();

                copy_index_data(item);
                transform_index_locations(start_index, end_index, m_vertex_buffer.active_vertex_count());
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                m_vertex_buffer.submit();

                if (active_index_count() != 0)
                {
                    //GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index_buffer_byte_size(), indices()));
                    m_index_buffer.submit();
                }
            }

            //-------------------------------------------------------------------------
            void reset()
            {
                m_vertex_buffer.free();
                m_index_buffer.free();
            }

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
            void copy_vertex_data(const irender_item* item, const glm::vec4& color)
            {
                vertex_buffer_ops::vertex_attribute_addition_scope vaas(m_vertex_buffer, item->vertex_count());

                if (m_vertex_buffer.has_layout(vertex_attribute_type::POSITION)) vertex_buffer_ops::set_attribute_data(vaas, vertex_attribute_type::POSITION, item->vertex_positions().data());
                if (m_vertex_buffer.has_layout(vertex_attribute_type::NORMAL)) vertex_buffer_ops::set_attribute_data(vaas, vertex_attribute_type::NORMAL, item->vertex_normals().data());
                if (m_vertex_buffer.has_layout(vertex_attribute_type::TEXCOORD)) vertex_buffer_ops::set_attribute_data(vaas, vertex_attribute_type::TEXCOORD, item->vertex_uvs().data());

                vertex_buffer_ops::map_attribute_data(vaas, vertex_attribute_type::COLOR, (void*)&color[0]);
            }

            //-------------------------------------------------------------------------
            void copy_index_data(const irender_item* item)
            {
                index_buffer_ops::index_addition_scope ias(m_index_buffer, item->index_count());

                assert(sizeof(item->faces()[0][0]) == sizeof(index) && "different index size was used");

                index_buffer_ops::set_index_data(ias, item->faces().data());
            }

            //-------------------------------------------------------------------------
            void transform_vertex_positions(s32 start_index, s32 end_index, const glm::mat4& world)
            {
                vertex_buffer_ops::transform_attribute_data<glm::vec3>(m_vertex_buffer, vertex_attribute_type::POSITION, start_index, end_index, [&](glm::vec3& position)
                {
                    glm::vec4 transformed_pos = world * glm::vec4(position, 1.0f);

                    position.x = transformed_pos.x;
                    position.y = transformed_pos.y;
                    position.z = transformed_pos.z;
                });
            }

            //-------------------------------------------------------------------------
            void transform_vertex_normals(s32 start_index, s32 end_index, const glm::mat4& world)
            {
                glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(world)));

                vertex_buffer_ops::transform_attribute_data<glm::vec3>(m_vertex_buffer, vertex_attribute_type::NORMAL, start_index, end_index, [&](glm::vec3& normal)
                {
                    glm::vec3 transformed_normal = normal_matrix * normal;

                    normal = glm::normalize(transformed_normal);
                });
            }

            //-------------------------------------------------------------------------
            void transform_vertex_diffuse_texture_ids(s32 start_index, s32 end_index, s32 sampler_id)
            {
                vertex_buffer_ops::transform_attribute_data<f32>(m_vertex_buffer, vertex_attribute_type::DIFFUSE_TEXTURE_INDEX, start_index, end_index, [&](float& id)
                {
                    id = (f32)sampler_id;
                });
            }
            
            //-------------------------------------------------------------------------
            void transform_index_locations(s32 start_index, s32 end_index, u64 offset)
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

        class batch::impl
        {
        public:
            //-------------------------------------------------------------------------
            impl(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count, s32 size_textures)
            {
                // Allocate VAO
                GL_CALL(glGenVertexArrays(1, &m_vao));
                GL_CALL(glBindVertexArray(m_vao));

                m_buffer_manager = std::make_unique<buffer_manager>(size_vertex_buffer, size_index_buffer, layouts, layout_count);
                m_texture_registry = std::make_unique<texture_registry>(size_textures);

                GL_CALL(glBindVertexArray(0));
            }

            //-------------------------------------------------------------------------
            ~impl()
            {   
                GL_CALL(glDeleteVertexArrays(1, &m_vao));
            }

            
            //-------------------------------------------------------------------------
            void bind() const
            {
                GL_CALL(glBindVertexArray(m_vao));
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                GL_CALL(glBindVertexArray(0));
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                m_buffer_manager->submit();
            }

            //-------------------------------------------------------------------------
            void draw(topology_type topology, u32 shader_program) const
            {
                GLenum gl_topology = internal::topology(topology);

                #ifndef NDEBUG
                internal::check_drawing_type(m_buffer_manager->active_index_count(), gl_topology);
                #endif

                shaders::apply_uniforms(shader_program);

                if (m_buffer_manager->active_index_count() != 0)
                {
                    GL_CALL(glDrawElements(gl_topology, m_buffer_manager->active_index_count(), internal::index_type(), nullptr));
                }
                else
                {
                    GL_CALL(glDrawArrays(gl_topology, 0, m_buffer_manager->active_vertex_count()));
                }
            }

            std::unique_ptr<buffer_manager> m_buffer_manager;
            std::unique_ptr<texture_registry> m_texture_registry;

            u32 m_vao;
        };

        //-------------------------------------------------------------------------
        batch::batch(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count, s32 size_textures)
            : m_pimpl(std::make_unique<impl>(size_vertex_buffer, size_index_buffer, layouts, layout_count, size_textures))
        {}
        //-------------------------------------------------------------------------
        batch::~batch() = default;
        //-------------------------------------------------------------------------
        batch::batch(batch&& other)
            : m_pimpl(std::exchange(other.m_pimpl, nullptr))
        {
        }
        //-------------------------------------------------------------------------
        batch& batch::operator=(batch&& other)
        {
            m_pimpl = std::exchange(other.m_pimpl, nullptr);

            return *this;
        }

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
        void batch::draw(topology_type topology, u32 shader_program) const
        {
            m_pimpl->draw(topology, shader_program);
        }

        //-------------------------------------------------------------------------
        void batch::append(const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {
            if (item->index_count() != 0)
            {
                m_pimpl->m_buffer_manager->add_indices(item);
            }
            
            if (m_pimpl->m_texture_registry->has_reserved_texture_space())
            {
                assert(item->texture_ids().size() == 1);

                s32 sampler_id = m_pimpl->m_texture_registry->add_texture(item->texture_ids()[0]);
                if (sampler_id != -1)
                {
                    m_pimpl->m_buffer_manager->add_vertices(item, sampler_id, color, world);
                }
                else
                {
                    log::error("Unable to generate sampler id for texture.");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                m_pimpl->m_buffer_manager->add_vertices(item, color, world);
            }
        }
        //-------------------------------------------------------------------------
        void batch::reset()
        {
            m_pimpl->m_buffer_manager->reset();
            m_pimpl->m_texture_registry->reset();
        }

        //-------------------------------------------------------------------------
        bool batch::can_add(s32 nr_vertices, s32 nr_indices) const
        {
            return m_pimpl->m_buffer_manager->can_add(nr_vertices, nr_indices);
        }
        //-------------------------------------------------------------------------
        bool batch::has_data() const
        {
            return m_pimpl->m_buffer_manager->has_data() || (m_pimpl->m_texture_registry->has_reserved_texture_space() && m_pimpl->m_texture_registry->has_data());
        }
        //-------------------------------------------------------------------------
        bool batch::has_reserved_texture_space() const { return m_pimpl->m_texture_registry->has_reserved_texture_space(); }

        //-------------------------------------------------------------------------
        const void* batch::vertices() const { return m_pimpl->m_buffer_manager->vertices(); }
        //-------------------------------------------------------------------------
        const void* batch::indices() const { return m_pimpl->m_buffer_manager->indices(); }
        //-------------------------------------------------------------------------
        const s32* batch::samplers() const { return has_reserved_texture_space() ? m_pimpl->m_texture_registry->samplers().data() : nullptr; }
        //-------------------------------------------------------------------------
        const u32* batch::textures() const { return has_reserved_texture_space() ? m_pimpl->m_texture_registry->textures().data() : nullptr; }

        //-------------------------------------------------------------------------
        u32 batch::active_vertex_count() const { return m_pimpl->m_buffer_manager->active_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 batch::active_index_count() const { return m_pimpl->m_buffer_manager->active_index_count(); }
        //-------------------------------------------------------------------------
        u32 batch::active_sampler_count() const { return m_pimpl->m_texture_registry->active_sampler_count(); }
        //-------------------------------------------------------------------------
        u32 batch::active_texture_count() const { return m_pimpl->m_texture_registry->active_texture_count(); }

        //-------------------------------------------------------------------------
        u64 batch::vertex_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_vertices_byte_size(); }
        //-------------------------------------------------------------------------
        u64 batch::index_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_indices_byte_size(); }

        //-------------------------------------------------------------------------
        u32 batch::max_vertex_count() const { return m_pimpl->m_buffer_manager->max_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 batch::max_index_count() const { return m_pimpl->m_buffer_manager->max_index_count(); }
        //-------------------------------------------------------------------------
        u32 batch::max_texture_count() const { return m_pimpl->m_texture_registry->max_texture_count(); }

        //-------------------------------------------------------------------------
        batch_drawing_data::batch_drawing_data(s32 size_vertex_buffer, s32 size_index_buffer, const vertex_attribute_layout* layouts, u64 layout_count, render_buffer_policy render_buffer_policy)
            : batch_drawing_data(size_vertex_buffer, size_index_buffer, -1, layouts, layout_count, render_buffer_policy)
        {
        }
        //-------------------------------------------------------------------------
        batch_drawing_data::batch_drawing_data(s32 size_vertex_buffer, s32 size_index_buffer, s32 size_textures, const vertex_attribute_layout* layouts, u64 layout_count, render_buffer_policy render_buffer_policy)
            : m_layouts(layouts)
            , m_layout_count(layout_count)
            , m_buffer_policy(render_buffer_policy)
        {
            assert(size_vertex_buffer > 0);
            assert(size_index_buffer > 0);

            assert(layouts != nullptr);
            assert(layout_count > 0);

            // Already start with one batch
            m_batches.emplace_back(size_vertex_buffer, size_index_buffer, layouts, layout_count, size_textures);
        }

        //-------------------------------------------------------------------------
        void batch_drawing_data::append(const irender_item* item, const glm::vec4& color, const glm::mat4& world)
        {           
            if (item->vertex_count() == 0)
            {
                log::error("render item does not have vertices");
                exit(EXIT_FAILURE);
            }
            
            if (m_batches[m_push_batch].can_add(item->vertex_count(), item->index_count()))
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
            // We clear everything if we are in immediate mode
            if (m_buffer_policy == render_buffer_policy::IMMEDIATE)
            {
                for (batch& b : m_batches)
                {
                    b.reset();
                }

                m_push_batch = 0;
            }

            m_draw_batch = 0;
        }
        //-------------------------------------------------------------------------
        void batch_drawing_data::release()
        {
            reset();
        }

        //-------------------------------------------------------------------------
        const batch* batch_drawing_data::first_batch()
        {
            m_draw_batch = 0;

            return next_batch();
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
        bool batch_drawing_data::has_drawing_data() const
        {
            return std::any_of(m_batches.cbegin(), m_batches.cend(), [](const batch& b) { return b.has_data(); });
        }
    }
}