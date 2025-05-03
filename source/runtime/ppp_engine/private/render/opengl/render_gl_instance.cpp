#include "render/render_instance.h"
#include "render/render_vertex_buffer.h"
#include "render/render_index_buffer.h"
#include "render/render_instance_buffer.h"
#include "render/render_storage_buffer.h"
#include "render/render_features.h"

#include "render/opengl/render_gl_api.h"
#include "render/opengl/render_gl_util.h"

#include "render/helpers/render_vertex_buffer_ops.h"
#include "render/helpers/render_index_buffer_ops.h"
#include "render/helpers/render_storage_buffer_ops.h"

#include "resources/material_pool.h"

#include "util/types.h"
#include "util/log.h"
#include "util/pointer_math.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp >

#include <numeric>

namespace ppp
{
    namespace render
    {
        using instance_map = std::vector<instance>;

        //-------------------------------------------------------------------------
        // Buffer Manager
        constexpr s32 s_instance_data_initial_capacity = 16;

        namespace instance_storage
        {
            //-------------------------------------------------------------------------
            // Unaligned size in bytes of the structure storage on the GPU
            static u64 size_in_bytes()
            {
                constexpr u64 total_size_in_bytes = sizeof(s32)   // material index
                    + sizeof(glm::mat4)                           // model matrix of the geometry
                    + sizeof(glm::vec4);                          // color of the geometry

                return total_size_in_bytes;
            }
        };

        class instance_buffer_manager
        {
        public:
            //-------------------------------------------------------------------------
            instance_buffer_manager(const irender_item* instance, const attribute_layout* layouts, u32 layout_count)
                : m_vertex_buffer(instance->vertex_count(), layouts, layout_count)
                , m_index_buffer(instance->index_count())
                , m_instance_buffer(s_instance_data_initial_capacity, instance_storage::size_in_bytes(), 0)
            {

            }

            //-------------------------------------------------------------------------
            bool has_data() const
            {
                return (m_vertex_buffer.active_element_count() > 0 || m_index_buffer.active_element_count() > 0) && m_instance_buffer.active_element_count() > 0;
            }

            //-------------------------------------------------------------------------
            void add_instance_data(s32 material_id, const glm::vec4& color, const glm::mat4& world)
            {
                copy_instance_data(material_id, color, world);
            }
            //-------------------------------------------------------------------------
            void add_vertices(const irender_item* item)
            {
                copy_vertex_data(item);
            }

            //-------------------------------------------------------------------------
            void add_indices(const irender_item* item)
            {
                assert(!item->faces().empty());

                u64 start_index = m_index_buffer.active_element_count();
                u64 end_index = start_index + item->index_count();

                copy_index_data(item);

                transform_index_locations(start_index, end_index, m_vertex_buffer.active_element_count());
            }

            //-------------------------------------------------------------------------
            void bind() const
            {
                m_instance_buffer.bind();
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                m_instance_buffer.unbind();
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                m_vertex_buffer.submit();
                m_instance_buffer.submit();

                if (active_index_count() != 0)
                {
                    m_index_buffer.submit();
                }
            }

            //-------------------------------------------------------------------------
            void reset() const
            {   
                // We do not reset the vertex and index buffer as we need to retain the information 
                //  we stored when we created this instance.
                // If we reset them the "has_data" function thinks there is no available vertex 
                //  ( and/or index ) information so it won't draw anything.
                // 
                // m_vertex_buffer.reset();
                // m_index_buffer.reset();

                m_instance_buffer.reset();
            }
            //-------------------------------------------------------------------------
            void release() const
            {
                m_vertex_buffer.free();
                m_index_buffer.free();
                m_instance_buffer.free();
            }

            //-------------------------------------------------------------------------
            u64 active_vertex_count() const { return m_vertex_buffer.active_element_count(); }
            u64 active_vertices_byte_size() const { return m_vertex_buffer.total_buffer_size_in_bytes(); }
            u64 active_index_count() const { return m_index_buffer.active_element_count(); }
            u64 active_indices_byte_size() const { return m_index_buffer.total_buffer_size_in_bytes(); }

            //-------------------------------------------------------------------------
            const void* vertices() const { return m_vertex_buffer.data(); }
            const void* indices() const { return m_index_buffer.data(); }

        private:
            //-------------------------------------------------------------------------
            void copy_vertex_data(const irender_item* item)
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
            }
            //-------------------------------------------------------------------------
            void copy_index_data(const irender_item* item)
            {
                index_buffer_ops::index_addition_scope ias(m_index_buffer, item->index_count());

                assert(sizeof(item->faces()[0][0]) == sizeof(index) && "different index size was used");

                index_buffer_ops::set_index_data(ias, item->faces().data());
            }
            //-------------------------------------------------------------------------
            void copy_instance_data(s32 material_id, const glm::vec4& color, const glm::mat4& world)
            {
                storage_buffer_ops::storage_data_addition_scope sdas(m_instance_buffer, 1);

                std::vector<u8> instance_data(m_instance_buffer.element_size_in_bytes());

                size_t offset = 0;

                offset = copy_material_index(material_id, instance_data.data(), offset);
                offset = copy_world_matrix(world, instance_data.data(), offset);
                offset = copy_color(color, instance_data.data(), offset);

                storage_buffer_ops::set_storage_data(sdas, instance_data.data());
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

        private:
            //-------------------------------------------------------------------------
            size_t copy_material_index(s32 material_index, u8* buffer, u64 offset)
            {
                // For more info on why alignment is 16 see:
                // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
                const s32 alignment = 4;

                offset = memory::align_up(offset, alignment); // Align for `vec4`.
                std::memcpy(buffer + offset, &material_index, sizeof(s32));
                offset += sizeof(s32);

                return offset;
            }
            //-------------------------------------------------------------------------
            size_t copy_world_matrix(const glm::mat4& world, u8* buffer, u64 offset)
            {
                // For more info on why alignment is 16 see:
                // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
                const s32 alignment = 16;

                offset = memory::align_up(offset, alignment); // Align for `mat4`.
                std::memcpy(buffer + offset, &world, sizeof(glm::mat4));
                offset += sizeof(glm::mat4);

                return offset;
            }
            //-------------------------------------------------------------------------
            size_t copy_color(const glm::vec4& color, u8* buffer, u64 offset)
            {
                // For more info on why alignment is 16 see:
                // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
                const s32 alignment = 16;

                offset = memory::align_up(offset, alignment); // Align for `vec4`.
                std::memcpy(buffer + offset, &color, sizeof(glm::vec4));
                offset += sizeof(glm::vec4);

                return offset;
            }

            vertex_buffer   m_vertex_buffer;
            index_buffer    m_index_buffer;
            storage_buffer  m_instance_buffer;
        };

        //-------------------------------------------------------------------------
        // Material Buffer Manager
        namespace instance_material_storage
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

        class instance_material_manager
        {
        public:
            //-------------------------------------------------------------------------
            instance_material_manager()
                :m_storage_buffer(8, instance_material_storage::size_in_bytes(), 1)
            {}

            //-------------------------------------------------------------------------
            s32 add_material_attributes(const irender_item* item)
            {
                const u32 material_index = m_storage_buffer.active_element_count();
                assert(material_index == static_cast<s32>(material_index));

                copy_material_data(item);

                return static_cast<s32>(material_index);
            }

            //-------------------------------------------------------------------------
            void bind() const
            {
                m_storage_buffer.bind();
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                m_storage_buffer.unbind();
            }

            //-------------------------------------------------------------------------
            void submit() const
            {
                m_storage_buffer.submit();
            }

            //-------------------------------------------------------------------------
            void reset() const
            {
                m_storage_buffer.reset();
            }

            //-------------------------------------------------------------------------
            void release() const
            {
                m_storage_buffer.free();
            }

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
        // Instance Impl
        class instance::impl
        {
        public:
            impl(const irender_item* instance, const attribute_layout* layouts, u32 layout_count)
                :m_instance_id(instance->geometry_id())
            {
                assert(layouts != nullptr);
                assert(layout_count > 0);

                assert(instance->vertex_count() > 0 && "render item does not have vertices");

                // Allocate VAO
                opengl::api::instance().generate_vertex_arrays(1, &m_vao);
                opengl::api::instance().bind_vertex_array(m_vao);

                m_buffer_manager = std::make_unique<instance_buffer_manager>(instance, layouts, layout_count);
                m_material_manager = std::make_unique<instance_material_manager>();

                if (instance->index_count() != 0)
                {
                    m_buffer_manager->add_indices(instance);
                }

                m_buffer_manager->add_vertices(instance);

                opengl::api::instance().bind_vertex_array(0);
            }

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

                m_buffer_manager->bind();
                m_material_manager->bind();
            }

            //-------------------------------------------------------------------------
            void unbind() const
            {
                m_material_manager->unbind();
                m_buffer_manager->unbind();

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
                GLenum gl_topology = gl_topology_type(topology);

#ifndef NDEBUG
                check_drawing_type(m_buffer_manager->active_index_count(), gl_topology);
#endif

                if (m_buffer_manager->active_index_count() != 0)
                {
                    opengl::api::instance().draw_elements_instanced(gl_topology, m_buffer_manager->active_index_count(), gl_index_type(), nullptr, m_instance_count);
                }
                else
                {
                    opengl::api::instance().draw_arrays_instanced(gl_topology, 0, m_buffer_manager->active_vertex_count(), m_instance_count);
                }
            }

            //-------------------------------------------------------------------------
            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world) const
            {
                if (item->has_textures())
                {
                    s32 material_id = m_material_manager->add_material_attributes(item);

                    if (material_id != -1)
                    {
                        m_buffer_manager->add_instance_data(material_id, color, world);
                    }
                    else
                    {
                        log::error("Unable to generate sampler id for texture.");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    m_buffer_manager->add_instance_data(-1, color, world);
                }
            }

            u64 m_instance_id = 0;
            s32 m_instance_count = 0;

            std::unique_ptr<instance_buffer_manager> m_buffer_manager;
            std::unique_ptr<instance_material_manager> m_material_manager;

            u32 m_vao;
        };

        //-------------------------------------------------------------------------
        // Instance
        instance::instance(const irender_item* instance, const attribute_layout* layouts, u32 layout_count)
            :m_pimpl(std::make_unique<impl>(instance, layouts, layout_count))
        {

        }
        //-------------------------------------------------------------------------
        instance::~instance() = default;
        //-------------------------------------------------------------------------
        instance::instance(instance&& other) noexcept = default;
        //-------------------------------------------------------------------------
        instance& instance::operator=(instance&& other) noexcept = default;

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
        void instance::draw(topology_type topology) const
        {
            m_pimpl->draw(topology);
        }

        //-------------------------------------------------------------------------
        void instance::increment_instance_count() const
        {
            ++m_pimpl->m_instance_count;
        }

        //-------------------------------------------------------------------------
        void instance::append(const irender_item* item, const glm::vec4& color, const glm::mat4& world) const
        {
            m_pimpl->append(item, color, world);
        }
        //-------------------------------------------------------------------------
        void instance::reset() const
        {
            m_pimpl->m_instance_count = 0;

            m_pimpl->m_buffer_manager->reset();
            m_pimpl->m_material_manager->reset();
        }
        //-------------------------------------------------------------------------
        void instance::release() const
        {
            m_pimpl->m_buffer_manager->release();
            m_pimpl->m_material_manager->release();
            m_pimpl->release();
        }

        //-------------------------------------------------------------------------
        bool instance::has_data() const
        {
            return m_pimpl->m_buffer_manager->has_data();
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
        u32 instance::active_vertex_count() const { return m_pimpl->m_buffer_manager->active_vertex_count(); }
        //-------------------------------------------------------------------------
        u32 instance::active_index_count() const { return m_pimpl->m_buffer_manager->active_index_count(); }

        //-------------------------------------------------------------------------
        u64 instance::vertex_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_vertices_byte_size(); }
        //-------------------------------------------------------------------------
        u64 instance::index_buffer_byte_size() const { return m_pimpl->m_buffer_manager->active_indices_byte_size(); }

        //-------------------------------------------------------------------------
        // Instance Drawing Data Impl
        struct instance_drawing_data::impl
        {
            impl(const attribute_layout* layouts, u32 layout_count)
                : layouts(layouts)
                , layout_count(layout_count)
            {

            }

            instance_map                instances                 = {};

            const attribute_layout*     layouts                   = nullptr;
            u32                         layout_count              = 0;

            s32                         draw_instance             = 0;
        };

        //-------------------------------------------------------------------------
        // Instance Drawing Data
        instance_drawing_data::instance_drawing_data(const attribute_layout* layouts, u32 layout_count)
            : m_pimpl(std::make_unique<impl>(layouts, layout_count))
        {
            assert(layouts != nullptr);
            assert(layout_count > 0);
        }
        //-------------------------------------------------------------------------
        instance_drawing_data::~instance_drawing_data() = default;
        //-------------------------------------------------------------------------
        instance_drawing_data::instance_drawing_data(instance_drawing_data&& other) noexcept = default;
        //-------------------------------------------------------------------------
        instance_drawing_data& instance_drawing_data::operator=(instance_drawing_data&& other) noexcept = default;

        //-------------------------------------------------------------------------
        void instance_drawing_data::append(const irender_item* item, const glm::vec4& color, const glm::mat4& world) const
        {
            auto it = std::find_if(std::begin(m_pimpl->instances), std::end(m_pimpl->instances),
                [item](const instance& inst)
            {
                bool is_equal = inst.instance_id() == item->geometry_id();
                return is_equal;
            });

            instance* new_instance;

            if (it == std::cend(m_pimpl->instances))
            {
                instance& inst = m_pimpl->instances.emplace_back(item, m_pimpl->layouts, m_pimpl->layout_count);
                
                new_instance = &inst;
            }
            else
            {
                new_instance = &(*it);
            }

            new_instance->increment_instance_count();
            new_instance->append(item, color, world);
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::reset() const
        {
            for (instance& b : m_pimpl->instances)
            {
                b.reset();
            }

            m_pimpl->draw_instance = 0;
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::release() const
        {
            for (instance& i : m_pimpl->instances)
            {
                i.reset();
                i.release();
            }

            m_pimpl->draw_instance = 0;
        }

        //-------------------------------------------------------------------------
        const instance* instance_drawing_data::first_instance() const
        {
            m_pimpl->draw_instance = 0;

            return next_instance();
        }

        //-------------------------------------------------------------------------
        const instance* instance_drawing_data::next_instance() const
        {
            if (m_pimpl->draw_instance < m_pimpl->instances.size())
            {
                auto b = &m_pimpl->instances[m_pimpl->draw_instance];

                ++m_pimpl->draw_instance;

                return b;
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        bool instance_drawing_data::has_drawing_data() const
        {
            return std::any_of(m_pimpl->instances.cbegin(), m_pimpl->instances.cend(), [](const instance& i) { return i.has_data(); });
        }
    }
}