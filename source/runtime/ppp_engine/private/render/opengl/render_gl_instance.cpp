#include "render/render_instance.h"

#include "render/opengl/render_gl_error.h"

#include "util/types.h"
#include "util/log.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static GLenum convert_to_gl_data_type(vertex_attribute_data_type type)
            {
                switch (type)
                {
                case vertex_attribute_data_type::FLOAT: return GL_FLOAT;
                case vertex_attribute_data_type::UNSIGNED_INT: return GL_UNSIGNED_INT;
                }
                return 0;  // Fallback to avoid compiler warnings
            }

            //-------------------------------------------------------------------------
            static u64 calculate_total_size_vertex_type(const vertex_attribute_layout* layouts, u64 layout_count)
            {
                u64 total = 0;
                for (u64 i = 0; i < layout_count; ++i)
                {
                    const vertex_attribute_layout& layout = layouts[i];

                    total += layout.total_size_in_bytes();
                }
                return total;
            }

            s32 s_instance_data_initial_capacity = 16;
        }

        //-------------------------------------------------------------------------
        instance_drawing_data::instance_drawing_data(topology_type topology, const irender_item* instance, const vertex_attribute_layout* layouts, u64 layout_count)
            : m_topology_type(topology)
            , m_instance(instance)
            , m_vertex_buffer(std::make_unique<vertex_buffer>(layouts, layout_count, instance->vertex_count()))
            , m_index_buffer(instance->index_count() > 0 ? std::make_unique<index_buffer>(instance->index_count()) : nullptr)
            , m_instance_buffer()
            , m_layouts(layouts)
            , m_layout_count(layout_count)
        {
            assert(layouts != nullptr);
            assert(layout_count > 0);

            // CPU
            // Vertex Bugger
            m_vertex_buffer->open_attribute_addition(instance->vertex_count());

            if (m_vertex_buffer->has_layout(vertex_attribute_type::POSITION)) m_vertex_buffer->set_attribute_data(vertex_attribute_type::POSITION, instance->vertex_positions().data());
            if (m_vertex_buffer->has_layout(vertex_attribute_type::NORMAL)) m_vertex_buffer->set_attribute_data(vertex_attribute_type::NORMAL, instance->vertex_normals().data());
            if (m_vertex_buffer->has_layout(vertex_attribute_type::TEXCOORD)) m_vertex_buffer->set_attribute_data(vertex_attribute_type::TEXCOORD, instance->vertex_uvs().data());

            glm::vec4 color_white = { 1.0f, 1.0f, 1.0f, 1.0f };
            m_vertex_buffer->map_attribute_data(vertex_attribute_type::COLOR, &color_white);

            m_vertex_buffer->close_attribute_addition();

            // Index Buffer
            if (instance->index_count() > 0)
            {
                m_index_buffer->set_index_data(reinterpret_cast<const u32*>(instance->faces().data()), instance->index_count());
            }

            // Instance Buffer
            m_instance_buffer.reserve(internal::s_instance_data_initial_capacity);

            // GPU
            // Allocate VAO
            GL_CALL(glGenVertexArrays(1, &m_vao));
            GL_CALL(glBindVertexArray(m_vao));

            // Allocate VBO
            GL_CALL(glGenBuffers(1, &m_vbo));
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
            const size_t size_vbo = internal::calculate_total_size_vertex_type(layouts, layout_count) * instance->vertex_count();
            GL_CALL(glBufferData(GL_ARRAY_BUFFER, size_vbo, m_vertex_buffer->get_data().data(), GL_STATIC_DRAW));

            for (u64 i = 0; i < layout_count; ++i)
            {
                const vertex_attribute_layout& layout = layouts[i];

                GL_CALL(glEnableVertexAttribArray(i));
                GL_CALL(glVertexAttribPointer(i, layout.count, internal::convert_to_gl_data_type(layout.data_type), layout.normalized ? GL_TRUE : GL_FALSE, layout.stride, (void*)layout.offset));
            }

            // Allocate EBO
            if (instance->index_count() > 0)
            {
                GL_CALL(glGenBuffers(1, &m_ebo));
                GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));
                const size_t size_ebo = sizeof(index) * instance->index_count();
                GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, m_index_buffer->get_data(), GL_STATIC_DRAW));
            }

            // Allocate Instance VBO
            GL_CALL(glGenBuffers(1, &m_ibo));
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_ibo));
            GL_CALL(glBufferData(GL_ARRAY_BUFFER, internal::s_instance_data_initial_capacity * sizeof(instance_data), nullptr, GL_DYNAMIC_DRAW)); // Allocate initial size

            // Setup Instance Attributes
            for (int i = 0; i < 4; ++i) 
            {
                GL_CALL(glEnableVertexAttribArray(layout_count + i));   // Attributes for mat4 (4 vec4s)
                GL_CALL(glVertexAttribPointer(layout_count + i, 4, GL_FLOAT, GL_FALSE, sizeof(instance_data), (void*)(i * sizeof(glm::vec4))));
                GL_CALL(glVertexAttribDivisor(layout_count + i, 1));    // Instanced attribute
            }

            GL_CALL(glEnableVertexAttribArray(layout_count + 4));       // Attribute for color
            GL_CALL(glVertexAttribPointer(layout_count + 4, 4, GL_FLOAT, GL_FALSE, sizeof(instance_data), (void*)offsetof(instance_data, color)));
            GL_CALL(glVertexAttribDivisor(layout_count + 4, 1));        // Instanced attribute

            GL_CALL(glBindVertexArray(0));
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::increment_instance_count()
        {
            m_instance_count++;
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::set(s32 instance_id, const glm::vec4& color, const glm::mat4& world)
        {
            if (!has_instance_data())
            {
                log::error("instance drawing data is invalid");
                exit(EXIT_FAILURE);
            }

            glBindBuffer(GL_ARRAY_BUFFER, m_ibo);

            assert(instance_id < m_instance_buffer.size());

            m_instance_buffer[instance_id].world = world;
            m_instance_buffer[instance_id].color = color;

            glBufferSubData(GL_ARRAY_BUFFER, 0, m_instance_count * sizeof(instance_data), m_instance_buffer.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::append(const glm::vec4& color, const glm::mat4& world)
        {
            if (!has_instance_data())
            {
                log::error("instance drawing data is invalid");
                exit(EXIT_FAILURE);
            }

            glBindBuffer(GL_ARRAY_BUFFER, m_ibo);

            if (m_instance_buffer.size() == m_instance_buffer.capacity())
            {
                s32 new_cap = m_instance_buffer.capacity() * 2;

                m_instance_buffer.reserve(new_cap);

                glBufferData(GL_ARRAY_BUFFER, new_cap * sizeof(instance_data), nullptr, GL_DYNAMIC_DRAW); // Allocate new GPU memory
            }

            instance_data inst_data = { world, color };
            
            glBufferSubData(GL_ARRAY_BUFFER, m_instance_data_offset, sizeof(instance_data), (void*)&inst_data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            m_instance_buffer.push_back(inst_data);
            m_instance_data_offset += sizeof(instance_data);
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::release()
        {
            m_vertex_buffer->free();
            m_index_buffer->free();
            m_instance_buffer.clear();

            GL_CALL(glDeleteBuffers(1, &m_vbo));
            GL_CALL(glDeleteBuffers(1, &m_ebo));
            GL_CALL(glDeleteBuffers(1, &m_ibo));
            GL_CALL(glDeleteVertexArrays(1, &m_vao));
        }

        //-------------------------------------------------------------------------
        void instance_drawing_data::reset()
        {
            m_vertex_buffer->free();
            m_index_buffer->free();
            m_instance_buffer.clear();

            m_instance_count = 0;
            m_instance_data_offset = 0;
        }

        //-------------------------------------------------------------------------
        const irender_item* instance_drawing_data::instance() const
        {
            return m_instance;
        }

        //-------------------------------------------------------------------------
        const topology_type instance_drawing_data::topology() const
        {
            return m_topology_type;
        }

        //-------------------------------------------------------------------------
        s32 instance_drawing_data::instance_count() const
        {
            return m_instance_count;
        }

        //-------------------------------------------------------------------------
        bool instance_drawing_data::has_instance_data() const
        {
            return m_instance != nullptr;
        }

        //-------------------------------------------------------------------------
        bool instance_drawing_data::has_drawing_data() const
        {
            return m_instance_buffer.size() > 0;
        }

        //-------------------------------------------------------------------------
        u32 instance_drawing_data::vao() const { return m_vao; }
        //-------------------------------------------------------------------------
        u32 instance_drawing_data::vbo() const { return m_vbo; }
        //-------------------------------------------------------------------------
        u32 instance_drawing_data::ebo() const { return m_ebo; }
        //-------------------------------------------------------------------------
        u32 instance_drawing_data::ibo() const { return m_ibo; }
    }
}