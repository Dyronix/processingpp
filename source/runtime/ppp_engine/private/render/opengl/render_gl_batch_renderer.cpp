#include "render/render_batch_renderer.h"
#include "render/render_item_components.h"

#include "resources/shader_pool.h"

#include "util/log.h"
#include "util/color_ops.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            void check_drawing_type(const batch* batch, GLenum type)
            {
                auto index_count = batch->active_index_count();
                switch (type)
                {
                case GL_LINES:
                    if (index_count % 2 != 0)
                    {
                        log::error("Trying to render invalid number of lines: {}", batch->active_index_count());
                        return;
                    }
                    break;
                case GL_TRIANGLES:
                    if (index_count % 3 != 0)
                    {
                        log::error("Trying to render invalid number of triangles: {}", batch->active_index_count());
                        return;
                    }
                    break;
                }
            }

            //-------------------------------------------------------------------------
            u32 index_type()
            {
                if (sizeof(index) == sizeof(u32)) return GL_UNSIGNED_INT;
                if (sizeof(index) == sizeof(u16)) return GL_UNSIGNED_SHORT;

                log::error("Invalid index type specified: {}, using UNSIGNED_INT", typeid(index).name());
                return GL_UNSIGNED_INT;
            }
        }

        // Batch Renderer
        //-------------------------------------------------------------------------
        f32 batch_renderer::s_wireframe_linewidth = 3.0f;
        s32 batch_renderer::s_wireframe_linecolor = 0x000000FF;

        //-------------------------------------------------------------------------
        void batch_renderer::set_wireframe_linewidth(f32 width) 
        {
            s_wireframe_linewidth = width; 
        }

        //-------------------------------------------------------------------------
        void batch_renderer::set_wireframe_linecolor(s32 color) 
        {
            s_wireframe_linecolor = color; 
        }

        //-------------------------------------------------------------------------
        batch_renderer::batch_renderer(batch_primitive_type primitive_type, s32 max_vertices, s32 max_indices, vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag)
            :batch_renderer(primitive_type, max_vertices, max_indices, -1, layouts, layout_cout, shader_tag)
        {}

        //-------------------------------------------------------------------------
        batch_renderer::batch_renderer(batch_primitive_type primitive_type, s32 max_vertices, s32 max_indices, s32 max_textures, vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag)
            : m_shader_tag(shader_tag)
            , m_rasterization_mode(s_solid)
            , m_drawing_data(max_vertices, max_indices, max_textures, layouts, layout_cout)
            , m_primitive_type(GL_TRIANGLES)
        {
            switch (primitive_type)
            {
            case batch_primitive_type::POINTS: m_primitive_type = GL_POINTS; break;
            case batch_primitive_type::LINES: m_primitive_type = GL_LINES; break;
            case batch_primitive_type::TRIANGLES: m_primitive_type = GL_TRIANGLES; break;
            }
        }

        //-------------------------------------------------------------------------
        batch_renderer::~batch_renderer() = default;

        //-------------------------------------------------------------------------
        void batch_renderer::begin()
        {
            m_drawing_data.reset();
        }

        //-------------------------------------------------------------------------
        void batch_renderer::render(const glm::mat4& vp)
        {
            // No drawing data, early out
            if (!m_drawing_data.has_drawing_data())
            {
                return;
            }

            glUseProgram(shader_program());
            u32 u_mpv_loc = glGetUniformLocation(shader_program(), "u_worldviewproj");
            glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

            if (solid_rendering_supported())
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glUniform1i(glGetUniformLocation(shader_program(), "u_wireframe"), GL_FALSE);

                on_render(m_drawing_data);
            }

            if (wireframe_rendering_supported())
            {
                m_drawing_data.load_first_batch();

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(s_wireframe_linewidth);
                glUniform1i(glGetUniformLocation(shader_program(), "u_wireframe"), GL_TRUE);
                glm::vec4 wireframe_color = color::convert_color(s_wireframe_linecolor);
                glUniform4fv(glGetUniformLocation(shader_program(), "u_wireframe_color"), 1, value_ptr(wireframe_color));

                on_render(m_drawing_data);
            }

            glUseProgram(0);
        }

        //-------------------------------------------------------------------------
        void batch_renderer::end()
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void batch_renderer::terminate()
        {
            m_drawing_data.release();
        }

        void batch_renderer::append_drawing_data(const render_item& item, const glm::vec4& color, const glm::mat4& world)
        {
            m_drawing_data.append(item, color, world);
        }

        //-------------------------------------------------------------------------
        void batch_renderer::enable_solid_rendering(bool enable)
        {
            if (enable)
            {
                m_rasterization_mode |= s_solid;
            }
            else
            {
                m_rasterization_mode &= ~s_solid;
            }
        }

        //-------------------------------------------------------------------------
        void batch_renderer::enable_wireframe_rendering(bool enable)
        {
            if (enable)
            {
                m_rasterization_mode |= s_wireframe;
            }
            else
            {
                m_rasterization_mode &= ~s_wireframe;
            }
        }

        //-------------------------------------------------------------------------
        bool batch_renderer::solid_rendering_supported() const 
        {
            return m_rasterization_mode & s_solid; 
        }

        //-------------------------------------------------------------------------
        bool batch_renderer::wireframe_rendering_supported() const 
        {
            return m_rasterization_mode & s_wireframe; 
        }

        //-------------------------------------------------------------------------
        u32 batch_renderer::shader_program() const 
        {
            return shader_pool::get_shader_program(m_shader_tag); 
        }

        //-------------------------------------------------------------------------
        u32 batch_renderer::primitive_type() const 
        {
            return m_primitive_type; 
        }

        // Primitive Batch Renderer
        //-------------------------------------------------------------------------
        primitive_batch_renderer::primitive_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag, s32 max_vertices, s32 max_indices, batch_primitive_type primitive_type)
            :batch_renderer(primitive_type, max_vertices, max_indices, layouts, layout_cout, shader_tag)
        {

        }

        //-------------------------------------------------------------------------
        primitive_batch_renderer::~primitive_batch_renderer() = default;

        //-------------------------------------------------------------------------
        void primitive_batch_renderer::on_render(batch_drawing_data& drawing_data)
        {
            auto batch = drawing_data.next_batch();
            if (batch != nullptr)
            {
                glBindVertexArray(drawing_data.vao());
                glBindBuffer(GL_ARRAY_BUFFER, drawing_data.vbo());

                while (batch != nullptr)
                {
                    #ifndef NDEBUG
                    internal::check_drawing_type(batch, primitive_type());
                    #endif

                    glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices());
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices());

                    glDrawElements(primitive_type(), batch->active_index_count(), internal::index_type(), nullptr);

                    batch = drawing_data.next_batch();
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

        // Texture Batch Renderer
        //-------------------------------------------------------------------------
        texture_batch_renderer::texture_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag, s32 max_vertices, s32 max_indices, s32 max_textures, batch_primitive_type primitive_type)
            :batch_renderer(primitive_type, max_vertices, max_indices, max_textures, layouts, layout_cout, shader_tag)
        {

        }

        //-------------------------------------------------------------------------
        texture_batch_renderer::~texture_batch_renderer() = default;

        //-------------------------------------------------------------------------
        void texture_batch_renderer::on_render(batch_drawing_data& drawing_data)
        {
            auto batch = drawing_data.next_batch();
            if (batch != nullptr)
            {
                glBindVertexArray(drawing_data.vao());
                glBindBuffer(GL_ARRAY_BUFFER, drawing_data.vbo());

                while (batch != nullptr)
                {
                    assert(batch->samplers());
                    assert(batch->textures());

                    u32 u_tex_samplers_loc = glGetUniformLocation(shader_program(), "s_image");
                    glUniform1iv(u_tex_samplers_loc, batch->active_sampler_count(), batch->samplers());

                    #ifndef NDEBUG
                    internal::check_drawing_type(batch, primitive_type());
                    #endif

                    s32 i = 0;
                    s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                    for (int i = 0; i < batch->active_texture_count(); ++i)
                    {
                        glActiveTexture(GL_TEXTURE0 + (offset * i));
                        glBindTexture(GL_TEXTURE_2D, batch->textures()[i]);
                    }

                    glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_buffer_byte_size(), batch->vertices());
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_buffer_byte_size(), batch->indices());

                    glDrawElements(primitive_type(), batch->active_index_count(), internal::index_type(), nullptr);

                    batch = drawing_data.next_batch();
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }
    }
}