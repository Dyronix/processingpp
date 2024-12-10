#include "render/render_instance.h"
#include "render/render_vertex_buffer.h"
#include "render/render_index_buffer.h"
#include "render/render_instance_buffer.h"
#include "render/render_shaders.h"

#include "render/opengl/render_gl_error.h"

#include "render/helpers/render_vertex_buffer_ops.h"
#include "render/helpers/render_index_buffer_ops.h"
#include "render/helpers/render_instance_buffer_ops.h"

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

            s32 s_instance_data_initial_capacity = 16;
        }

        //-------------------------------------------------------------------------
        // Buffer Manager
        class buffer_manager
        {
        public:
            //-------------------------------------------------------------------------
            buffer_manager(const irender_item* instance, const attribute_layout* layouts, u64 layout_count, const attribute_layout* instance_layouts, u64 instance_layout_count)
                : m_vertex_buffer(layouts, layout_count, instance->vertex_count())
                , m_index_buffer(instance->index_count())
                , m_instance_buffer(instance_layouts, instance_layout_count, internal::s_instance_data_initial_capacity)
            {

            }

            //-------------------------------------------------------------------------
            bool has_data() const
            {
                return (m_vertex_buffer.active_vertex_count() > 0 || m_index_buffer.active_index_count() > 0) && m_instance_buffer.active_instance_count() > 0;
            }

            //-------------------------------------------------------------------------
            void add_instance_data(const void* instance_data_ptr)
            {
                copy_instance_data(instance_data_ptr);
            }
            //-------------------------------------------------------------------------
            void add_vertices(const irender_item* item)
            {
                copy_vertex_data(item);
            }
            //-------------------------------------------------------------------------
            void add_vertices(const irender_item* item, s32 sampler_id)
            {
                s32 start_index = m_vertex_buffer.active_vertex_count();
                s32 end_index = start_index + item->vertex_count();

                add_vertices(item);

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
            void submit()
            {
                m_vertex_buffer.submit();
                m_instance_buffer.submit();

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
                m_instance_buffer.reset();
            }
            //-------------------------------------------------------------------------
            void release()
            {
                m_vertex_buffer.free();
                m_index_buffer.free();
                m_instance_buffer.free();
            }

            //-------------------------------------------------------------------------
            u32 active_vertex_count() const { return m_vertex_buffer.active_vertex_count(); }
            u64 active_vertices_byte_size() const { return m_vertex_buffer.total_size_in_bytes(); }
            u32 active_index_count() const { return m_index_buffer.active_index_count(); }
            u64 active_indices_byte_size() const { return m_index_buffer.total_size_in_bytes(); }

            //-------------------------------------------------------------------------
            const void* vertices() const { return m_vertex_buffer.data(); }
            const void* indices() const { return m_index_buffer.data(); }

        private:
            //-------------------------------------------------------------------------
            void copy_vertex_data(const irender_item* item)
            {
                vertex_buffer_ops::vertex_attribute_addition_scope vaas(m_vertex_buffer, item->vertex_count());

                if (m_vertex_buffer.has_layout(attribute_type::POSITION)) vertex_buffer_ops::set_attribute_data(vaas, attribute_type::POSITION, item->vertex_positions().data());
                if (m_vertex_buffer.has_layout(attribute_type::NORMAL)) vertex_buffer_ops::set_attribute_data(vaas, attribute_type::NORMAL, item->vertex_normals().data());
                if (m_vertex_buffer.has_layout(attribute_type::TEXCOORD)) vertex_buffer_ops::set_attribute_data(vaas, attribute_type::TEXCOORD, item->vertex_uvs().data());

                // TODO: Change this, we should not allocated per vertex color data if we will not use it
                glm::vec4 color_white = { 1.0f, 1.0f, 1.0f, 1.0f };
                vertex_buffer_ops::map_attribute_data(vaas, attribute_type::COLOR, (void*)&color_white[0]);
            }
            //-------------------------------------------------------------------------
            void copy_index_data(const irender_item* item)
            {
                index_buffer_ops::index_addition_scope ias(m_index_buffer, item->index_count());

                assert(sizeof(item->faces()[0][0]) == sizeof(index) && "different index size was used");

                index_buffer_ops::set_index_data(ias, item->faces().data());
            }
            //-------------------------------------------------------------------------
            void copy_instance_data(const void* data_ptr)
            {
                instance_buffer_ops::instance_addition_scope ias(m_instance_buffer, 1);
                
                instance_buffer_ops::set_instance_data(ias, data_ptr);
            }

            //-------------------------------------------------------------------------
            void transform_vertex_diffuse_texture_ids(s32 start_index, s32 end_index, s32 sampler_id)
            {
                vertex_buffer_ops::transform_attribute_data<f32>(m_vertex_buffer, attribute_type::DIFFUSE_TEXTURE_INDEX, start_index, end_index, [&](float& id)
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

            vertex_buffer m_vertex_buffer;
            index_buffer m_index_buffer;
            instance_buffer m_instance_buffer;
        };

        //-------------------------------------------------------------------------
        // Instance Impl
        class instance::impl
        {
        public:
            impl(const irender_item* instance, const attribute_layout* layouts, u64 layout_count, const attribute_layout* instance_layouts, u64 instance_layout_count, s32 size_textures)
                :m_instance_count(instance->id())
            {
                assert(layouts != nullptr);
                assert(layout_count > 0);

                assert(instance_layouts != nullptr);
                assert(instance_layout_count > 0);

                assert(instance->vertex_count() == 0 && "render item does not have vertices");

                // Allocate VAO
                GL_CALL(glGenVertexArrays(1, &m_vao));
                GL_CALL(glBindVertexArray(m_vao));

                m_buffer_manager = std::make_unique<buffer_manager>(instance, layouts, layout_count, instance_layouts, instance_layout_count);
                m_texture_registry = std::make_unique<texture_registry>(size_textures);

                if (instance->index_count() != 0)
                {
                    m_buffer_manager->add_indices(instance);
                }

                if (m_texture_registry->has_reserved_texture_space())
                {
                    assert(instance->texture_ids().size() == 1 && "Currently only one texture per render item is supported");

                    s32 sampler_id = m_texture_registry->add_texture(instance->texture_ids()[0]);
                    if (sampler_id != -1)
                    {
                        m_buffer_manager->add_vertices(instance, sampler_id);
                    }
                    else
                    {
                        log::error("Unable to generate sampler id for texture.");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    m_buffer_manager->add_vertices(instance);
                }

                GL_CALL(glBindVertexArray(0));
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
                    GL_CALL(glDrawElementsInstanced(gl_topology, m_buffer_manager->active_index_count(), internal::index_type(), nullptr, m_instance_count));
                }
                else
                {
                    GL_CALL(glDrawArraysInstanced(gl_topology, 0, m_buffer_manager->active_vertex_count(), m_instance_count));
                }
            }

            //-------------------------------------------------------------------------
            void append(const void* instance_data_ptr)
            {
                m_buffer_manager->add_instance_data(instance_data_ptr);
            }

            u64 m_instance_id = 0;
            s32 m_instance_count = 0;

            std::unique_ptr<buffer_manager> m_buffer_manager;
            std::unique_ptr<texture_registry> m_texture_registry;

            u32 m_vao;
        };

        //-------------------------------------------------------------------------
        // Instance
        instance::instance(const irender_item* instance, const attribute_layout* layouts, u64 layout_count, const attribute_layout* instance_layouts, u64 instance_layout_count)
            :m_pimpl(std::make_unique<impl>(instance, layouts, layout_count, instance_layouts, instance_layout_count))
        {

        }
        //-------------------------------------------------------------------------
        instance::~instance() = default;
        //-------------------------------------------------------------------------
        instance::instance(instance&& other)
            : m_pimpl(std::exchange(other.m_pimpl, nullptr))
        {
        }
        //-------------------------------------------------------------------------
        instance& instance::operator=(instance&& other)
        {
            m_pimpl = std::exchange(other.m_pimpl, nullptr);

            return *this;
        }

        //-------------------------------------------------------------------------
        void instance::bind() const
        {
            m_pimpl->bind();
        }
        //-------------------------------------------------------------------------
        void instance::unbind() const
        {
            m_pimpl->unbind();
        }
        //-------------------------------------------------------------------------
        void instance::submit() const
        {
            m_pimpl->submit();
        }
        //-------------------------------------------------------------------------
        void instance::draw(topology_type topology, u32 shader_program) const
        {
            m_pimpl->draw(topology, shader_program);
        }

        //-------------------------------------------------------------------------
        void instance::increment_instance_count()
        {
            ++m_pimpl->m_instance_count;
        }

        //-------------------------------------------------------------------------
        void instance::append(const void* data_ptr)
        {
            m_pimpl->append(data_ptr);
        }
        //-------------------------------------------------------------------------
        void instance::reset()
        {
            m_pimpl->m_buffer_manager->reset();
            m_pimpl->m_texture_registry->reset();
        }
        //-------------------------------------------------------------------------
        void instance::release()
        {
            m_pimpl->m_buffer_manager->release();
            m_pimpl->m_texture_registry->release();
        }

        //-------------------------------------------------------------------------
        bool instance::has_data() const
        {
            return m_pimpl->m_buffer_manager->has_data() || (m_pimpl->m_texture_registry->has_reserved_texture_space() && m_pimpl->m_texture_registry->has_data());
        }
        //-------------------------------------------------------------------------
        bool instance::has_reserved_texture_space() const 
        {
            return m_pimpl->m_texture_registry->has_reserved_texture_space(); 
        }

        //-------------------------------------------------------------------------
        u64 instance::instance_id() const
        {
            return m_pimpl->m_instance_id;
        }

        //-------------------------------------------------------------------------
        const void* instance::vertices() const { return m_pimpl->m_buffer_manager->vertices(); }
        //-------------------------------------------------------------------------
        const void* instance::indices() const { return m_pimpl->m_buffer_manager->indices(); }
        //-------------------------------------------------------------------------
        const s32* instance::samplers() const { return has_reserved_texture_space() ? m_pimpl->m_texture_registry->samplers().data() : nullptr; }
        //-------------------------------------------------------------------------
        const u32* instance::textures() const { return has_reserved_texture_space() ? m_pimpl->m_texture_registry->textures().data() : nullptr; }

        //-------------------------------------------------------------------------
        u32 instance::active_vertex_count() const { return m_pimpl->m_buffer_manager->active_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 instance::active_index_count() const { return m_pimpl->m_buffer_manager->active_index_count(); }
        //-------------------------------------------------------------------------
        u32 instance::active_sampler_count() const { return m_pimpl->m_texture_registry->active_sampler_count(); }
        //-------------------------------------------------------------------------
        u32 instance::active_texture_count() const { return m_pimpl->m_texture_registry->active_texture_count(); }

        //-------------------------------------------------------------------------
        u64 instance::vertex_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_vertices_byte_size(); }
        //-------------------------------------------------------------------------
        u64 instance::index_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_indices_byte_size(); }

        //-------------------------------------------------------------------------
        // Instance Drawing Data
        instance_drawing_data::instance_drawing_data(const attribute_layout* layouts, u64 layout_count, const attribute_layout* instance_layouts, u64 instance_layout_count, render_buffer_policy render_buffer_policy)
            : m_instances()
            , m_buffer_policy(render_buffer_policy)
            , m_layouts(layouts)
            , m_layout_count(layout_count)
            , m_instance_layouts(instance_layouts)
            , m_instance_layout_count(instance_layout_count)
        {
            assert(layouts != nullptr);
            assert(layout_count > 0);

            assert(instance_layouts != nullptr);
            assert(instance_layout_count > 0);
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::append(const irender_item* item, const void* instance_data_ptr)
        {
            auto it = std::find_if(std::begin(m_instances), std::end(m_instances),
                [item](const instance& inst)
            {
                return inst.instance_id() == item->id();
            });

            if (it == std::cend(m_instances))
            {
                instance& inst = m_instances.emplace_back(item, m_layouts, m_layout_count, m_instance_layouts, m_instance_layout_count);

                inst.increment_instance_count();
                inst.append(instance_data_ptr);
            }
            else
            {
                it->increment_instance_count();
                it->append(instance_data_ptr);
            }
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::reset()
        {
            if (m_buffer_policy == render_buffer_policy::IMMEDIATE)
            {
                for (instance& b : m_instances)
                {
                    b.reset();
                }
            }

            m_draw_instance = 0;
        }


        //-------------------------------------------------------------------------
        void instance_drawing_data::release()
        {
            for (instance& b : m_instances)
            {
                b.reset();
            }

            m_draw_instance = 0;
        }

        //-------------------------------------------------------------------------
        const instance* instance_drawing_data::first_instance()
        {
            m_draw_instance = 0;

            return next_instance();
        }

        //-------------------------------------------------------------------------
        const instance* instance_drawing_data::next_instance()
        {
            if (m_draw_instance < m_instances.size())
            {
                auto b = &m_instances[m_draw_instance];

                ++m_draw_instance;

                return b;
            }
        }

        //-------------------------------------------------------------------------
        bool instance_drawing_data::has_drawing_data() const
        {
            return std::any_of(m_instances.cbegin(), m_instances.cend(), [](const instance& i) { return i.has_data(); });
        }
    }
}