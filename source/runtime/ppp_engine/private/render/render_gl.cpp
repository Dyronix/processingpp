#include "render/render.h"
#include "render/render_gl_batch.h"
#include "render_transform.h"
#include "util/log.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <vector>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            u32 index_type()
            {
                if (typeid(Index).hash_code() == typeid(u32).hash_code()) return GL_UNSIGNED_INT;
                if (typeid(Index).hash_code() == typeid(u16).hash_code()) return GL_UNSIGNED_SHORT;

                log::error("Invalid index type specified: {}, using UNSIGNED_INT", typeid(Index).name());
                return GL_UNSIGNED_INT;
            }

            u32 convert_color(const glm::vec4 color)
            {
                u32 out;
                out = (conversions::f32_to_uint8(color.x)) << 24;
                out |= (conversions::f32_to_uint8(color.y)) << 16;
                out |= (conversions::f32_to_uint8(color.z)) << 8;
                out |= (conversions::f32_to_uint8(color.w)) << 0;
                return out;
            }

            glm::vec4 convert_color(u32 color)
            {
                f32 scale = 1.0f / 255.0f;
                return glm::vec4(
                    ((color >> 24) & 0xFF) * scale,
                    ((color >> 16) & 0xFF) * scale,
                    ((color >> 8) & 0xFF) * scale,
                    ((color >> 0) & 0xFF) * scale);
            }

            s32 _scissor_x = -1;
            s32 _scissor_y = -1;
            s32 _scissor_width = -1;
            s32 _scissor_height = -1;
            bool _scissor_enable = false;

            s32 _fill_color = 0xFFFFFFFF;
            bool _fill_enable = true;
            f32 _stroke_width = 1.0f;
            s32 _stroke_color = 0xFF000000;
            bool _stroke_enable = false;
            f32 _inner_stroke_width = 1.0f;
            s32 _inner_stroke_color = 0xFF000000;
            bool _inner_stroke_enable = false;
            s32 _tint_color = 0xFFFFFFFF;
            bool _tint_enable = false;

            s32 _bg_color = 0xFF000000;

            struct point_vertex_format
            {
                glm::vec3 position;
                glm::vec4 color;
            };
            struct line_vertex_format
            {
                glm::vec3 position;
                glm::vec4 color;
            };
            struct triangle_vertex_format
            {
                glm::vec3 position;
                glm::vec4 color;
            };
            struct image_vertex_format
            {
                glm::vec3 position;
                glm::vec2 texcoord;
                glm::vec4 color;
                f32       texture_idx;
            };
            struct font_vertex_format
            {
                glm::vec3 position;
                glm::vec2 texcoord;
                glm::vec4 color;
                f32       texture_idx;
            };

            bool compile_shader(GLuint* shader, GLenum type, const GLchar* source)
            {
                GLint status;

                if (!source)
                {
                    log::error("Failed to compile empty shader");
                    return false;
                }

                *shader = glCreateShader(type);
                glShaderSource(*shader, 1, &source, nullptr);

                glCompileShader(*shader);

                #ifndef NDEBUG
                GLint log_length = 0;
                glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &log_length);
                if (log_length > 1)
                {
                    GLchar* log = static_cast<GLchar*>(malloc(log_length));
                    glGetShaderInfoLog(*shader, log_length, &log_length, log);
                    if (log)
                        log::error("Program compile log: {}", log);
                    free(log);
                }
                #endif

                glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
                if (status == 0)
                {
                    glDeleteShader(*shader);
                    return false;
                }

                return true;
            }
            bool link_program(GLuint program)
            {
                GLint status;

                glLinkProgram(program);

                #if defined(DEBUG)
                GLint logLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
                if (logLength > 1)
                {
                    GLchar* log = static_cast<GLchar*>(malloc(logLength));
                    glGetProgramInfoLog(program, logLength, &logLength, log);
                    if (log)
                        log::error("Program link log: {}", log);
                    free(log);
                }
                #endif

                glGetProgramiv(program, GL_LINK_STATUS, &status);
                return status != 0;
            }

            constexpr s32 _max_points = 9'000;
            constexpr s32 _max_lines = 9'000;
            constexpr s32 _max_triangles = 9'000;
            constexpr s32 _max_texture_units = 8;

            constexpr s32 _min_frame_buffer_width = 32;
            constexpr s32 _min_frame_buffer_height = 32;

            s32 _frame_buffer_width = -1;
            s32 _frame_buffer_height = -1;

            u32 _render_fbo;
            u32 _render_texture;

            void create_frame_buffer()
            {
                glGenFramebuffers(1, &_render_fbo);
                glBindFramebuffer(GL_FRAMEBUFFER, _render_fbo);

                glGenTextures(1, &_render_texture);
                glBindTexture(GL_TEXTURE_2D, _render_texture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _frame_buffer_width, _frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture, 0);

                u32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
                glDrawBuffers(1, attachments);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    log::error("FramebufferStatus != FRAMEBUFFER_COMPLETE");
                    return;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            u32	_color_shader_program = 0;
            u32 _image_shader_program = 0;
            u32 _font_shader_program = 0;

            void compile_color_shader_program()
            {
                const auto* const vs_source =
                    "#version 460 core												\n\
		            layout (location = 0) in vec4 a_position;						\n\
		            layout (location = 1) in vec4 a_color;							\n\
		            layout (location = 2) uniform mat4 u_worldviewproj;				\n\
		            out vec4 v_color;												\n\
																		            \n\
		            void main()														\n\
		            {																\n\
		            	v_color = a_color;											\n\
		            	gl_Position = u_worldviewproj * a_position, 1.0;			\n\
		            }";

                const auto* const fs_source =
                    "#version 460 core												\n\
		            in vec4 v_color;												\n\
		            out vec4 frag_color;											\n\
		            																\n\
		            void main()														\n\
		            {																\n\
		            	frag_color = v_color;										\n\
		            }";

                GLuint vert_shader = 0;
                GLuint frag_shader = 0;

                _color_shader_program = glCreateProgram();

                GLboolean res = compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile vertex shader");
                    return;
                }

                res = compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile fragment shader");
                    return;
                }

                glAttachShader(_color_shader_program, vert_shader);
                glAttachShader(_color_shader_program, frag_shader);

                if (!link_program(_color_shader_program))
                {
                    glDeleteShader(vert_shader);
                    glDeleteShader(frag_shader);
                    glDeleteProgram(_color_shader_program);
                    log::error("Renderer failed to link shader program");
                    return;
                }

                glDeleteShader(vert_shader);
                glDeleteShader(frag_shader);
            }
            void compile_image_shader_program()
            {
                const auto* const vs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                   \n\
                    layout (location = 1) in vec2 a_texture;                                    \n\
                    layout (location = 2) in vec4 a_tint_color;                                 \n\
                    layout (location = 3) in float a_texture_idx;                               \n\
                    layout (location = 4) uniform mat4 u_worldviewproj;                         \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec2 v_texture;                                                         \n\
                    out float v_texture_idx;                                                    \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {						                                                    \n\
                        v_tint_color = a_tint_color;                                            \n\
                        v_texture = a_texture;                                                  \n\
                        v_texture_idx = a_texture_idx;                                          \n\
                    	gl_Position = u_worldviewproj * vec4(a_position, 1.0);                  \n\
                    }";

                const auto* const fs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (binding = 0) uniform sampler2D s_image[8];                          \n\
                                                                                                \n\
                    in vec4 v_tint_color;                                                       \n\
                    in vec2 v_texture;                                                          \n\
                    in float v_texture_idx;                                                     \n\
                                                                                                \n\
                    out vec4 frag_color;                                                        \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {                                                                           \n\
                        int idx = int(v_texture_idx);                                           \n\
                        vec4 color = texture(s_image[idx], v_texture) * v_tint_color;           \n\
                        frag_color = color;                                                     \n\
                    }";

                GLuint vert_shader = 0;
                GLuint frag_shader = 0;

                _image_shader_program = glCreateProgram();

                GLboolean res = compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile vertex shader");
                    return;
                }

                res = compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile fragment shader");
                    return;
                }

                glAttachShader(_image_shader_program, vert_shader);
                glAttachShader(_image_shader_program, frag_shader);

                if (!link_program(_image_shader_program))
                {
                    glDeleteShader(vert_shader);
                    glDeleteShader(frag_shader);
                    glDeleteProgram(_image_shader_program);
                    log::error("Renderer failed to link shader program");
                    return;
                }

                glDeleteShader(vert_shader);
                glDeleteShader(frag_shader);
            }
            void compile_font_shader_program()
            {
                const auto* const vs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                   \n\
                    layout (location = 1) in vec2 a_texture;                                    \n\
                    layout (location = 2) in vec4 a_tint_color;                                 \n\
                    layout (location = 3) in float a_texture_idx;                               \n\
                    layout (location = 4) uniform mat4 u_worldviewproj;                         \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec2 v_texture;                                                         \n\
                    out float v_texture_idx;                                                    \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {						                                                    \n\
                        v_tint_color = a_tint_color;                                            \n\
                        v_texture = a_texture;                                                  \n\
                        v_texture_idx = a_texture_idx;                                          \n\
                    	gl_Position = u_worldviewproj * vec4(a_position, 1.0);                  \n\
                    }";

                const auto* const fs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (binding = 0) uniform sampler2D s_image[8];                          \n\
                                                                                                \n\
                    in vec4 v_tint_color;                                                       \n\
                    in vec2 v_texture;                                                          \n\
                    in float v_texture_idx;                                                     \n\
                                                                                                \n\
                    out vec4 frag_color;                                                        \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {                                                                           \n\
                        int idx = int(v_texture_idx);                                           \n\
                        vec4 color = vec4(v_tint_color.rgb, texture(s_image[idx], v_texture).r);\n\
                        frag_color = color;                                                     \n\
                    }";

                GLuint vert_shader = 0;
                GLuint frag_shader = 0;

                _font_shader_program = glCreateProgram();

                GLboolean res = compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile vertex shader");
                    return;
                }

                res = compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile fragment shader");
                    return;
                }

                glAttachShader(_font_shader_program, vert_shader);
                glAttachShader(_font_shader_program, frag_shader);

                if (!link_program(_font_shader_program))
                {
                    glDeleteShader(vert_shader);
                    glDeleteShader(frag_shader);
                    glDeleteProgram(_font_shader_program);
                    log::error("Renderer failed to link shader program");
                    return;
                }

                glDeleteShader(vert_shader);
                glDeleteShader(frag_shader);
            }

            std::unique_ptr<PrimitiveDrawingData<point_vertex_format>> _points_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<point_vertex_format>> _points_stroke_drawing_data;

            std::unique_ptr<PrimitiveDrawingData<line_vertex_format>> _lines_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<line_vertex_format>> _lines_stroke_drawing_data;

            std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>> _triangle_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>> _triangle_stroke_drawing_data;

            std::unique_ptr<TextureDrawingData<image_vertex_format>> _image_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>> _image_stroke_drawing_data;

            std::unique_ptr<TextureDrawingData<image_vertex_format>> _font_drawing_data;

            void draw_points(const std::unique_ptr<PrimitiveDrawingData<point_vertex_format>>& point_drawing_data)
            {
                auto points_batch = point_drawing_data->next_batch();
                if (points_batch != nullptr)
                {
                    glBindVertexArray(point_drawing_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, point_drawing_data->vbo());

                    while (points_batch != nullptr)
                    {
                        glBufferSubData(GL_ARRAY_BUFFER, 0, points_batch->vertex_buffer_byte_size(), points_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, points_batch->index_buffer_byte_size(), points_batch->indices());

                        glDrawElements(GL_POINTS, points_batch->active_index_count(), internal::index_type(), nullptr);

                        points_batch = point_drawing_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
            void draw_lines(const std::unique_ptr<PrimitiveDrawingData<line_vertex_format>>& line_drawing_data)
            {
                auto lines_batch = line_drawing_data->next_batch();
                if (lines_batch != nullptr)
                {
                    glBindVertexArray(line_drawing_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, line_drawing_data->vbo());

                    while (lines_batch != nullptr)
                    {
                        #ifndef NDEBUG
                        if (lines_batch->active_index_count() % 2 != 0)
                        {
                            log::error("Trying to render invalid number of lines: {}", lines_batch->active_index_count());
                            return;
                        }
                        #endif

                        glBufferSubData(GL_ARRAY_BUFFER, 0, lines_batch->vertex_buffer_byte_size(), lines_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, lines_batch->index_buffer_byte_size(), lines_batch->indices());

                        glDrawElements(GL_LINES, lines_batch->active_index_count(), internal::index_type(), nullptr);

                        lines_batch = line_drawing_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
            void draw_triangles(const std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>>& triangle_drawing_data)
            {
                auto triangle_batch = triangle_drawing_data->next_batch();
                if (triangle_batch != nullptr)
                {
                    glBindVertexArray(triangle_drawing_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, triangle_drawing_data->vbo());

                    while (triangle_batch != nullptr)
                    {
                        #ifndef NDEBUG
                        if (triangle_batch->active_index_count() % 3 != 0)
                        {
                            log::error("Trying to render invalid number of triangles: {}", triangle_batch->active_index_count());
                            return;
                        }
                        #endif

                        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle_batch->vertex_buffer_byte_size(), triangle_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, triangle_batch->index_buffer_byte_size(), triangle_batch->indices());

                        glDrawElements(GL_TRIANGLES, triangle_batch->active_index_count(), internal::index_type(), nullptr);

                        triangle_batch = triangle_drawing_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
            void draw_images(const std::unique_ptr<TextureDrawingData<image_vertex_format>>& image_draw_data, s32 shader_program)
            {
                auto image_batch = image_draw_data->next_batch();
                if (image_batch != nullptr)
                {
                    glBindVertexArray(image_draw_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, image_draw_data->vbo());

                    while (image_batch != nullptr)
                    {
                        u32 u_tex_samplers_loc = glGetUniformLocation(shader_program, "s_image");
                        glUniform1iv(u_tex_samplers_loc, image_batch->active_texture_count(), image_batch->samplers());

                        #ifndef NDEBUG
                        if (image_batch->active_index_count() % 3 != 0)
                        {
                            log::error("Trying to render invalid number of triangles: {}", image_batch->active_index_count());
                            return;
                        }
                        #endif


                        s32 i = 0;
                        s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                        for (const auto& pair : image_batch->image_ids())
                        {
                            u32 id = pair.first;
                            s32 idx = pair.second;

                            glActiveTexture(GL_TEXTURE0 + (offset * i));
                            glBindTexture(GL_TEXTURE_2D, id);

                            ++i;
                        }

                        glBufferSubData(GL_ARRAY_BUFFER, 0, image_batch->vertex_buffer_byte_size(), image_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, image_batch->index_buffer_byte_size(), image_batch->indices());

                        glDrawElements(GL_TRIANGLES, image_batch->active_index_count(), internal::index_type(), nullptr);

                        image_batch = image_draw_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }

            u32 _white_texture_image_id;
            u32 _black_texture_image_id;
        }

        bool initialize(s32 w, s32 h, void* user_data)
        {
            // glad: load all OpenGL function pointers```
            // ---------------------------------------
            if (!gladLoadGLLoader((GLADloadproc)user_data))
            {
                log::error("Failed to initialize GLAD");
                return false;
            }

            internal::_frame_buffer_width = w;
            internal::_frame_buffer_height = h;

            internal::_scissor_x = 0;
            internal::_scissor_y = 0;
            internal::_scissor_width = w;
            internal::_scissor_height = h;
            internal::_scissor_enable = false;

            internal::compile_color_shader_program();
            internal::compile_image_shader_program();
            internal::compile_font_shader_program();
            internal::create_frame_buffer();

            // Primitive Drawing Data
            s32 max_vertex_elements = 0;
            s32 max_index_elements = 0;
            glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertex_elements);
            glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_index_elements);

            s32 max_point_vertices = std::min(max_vertex_elements, internal::_max_points);
            s32 max_point_indices = std::min(max_index_elements, internal::_max_points * 1);
            s32 max_line_vertices = std::min(max_vertex_elements, internal::_max_lines);
            s32 max_line_indices = std::min(max_index_elements, internal::_max_lines * 2);
            s32 max_triangle_vertices = std::min(max_vertex_elements, internal::_max_triangles);
            s32 max_triangle_indices = std::min(max_index_elements, internal::_max_triangles * 3);

            internal::_points_drawing_data = std::make_unique<PrimitiveDrawingData<internal::point_vertex_format>>(max_point_vertices, max_point_indices);
            internal::_points_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::point_vertex_format>>(max_point_vertices, max_point_indices);
            internal::_lines_drawing_data = std::make_unique<PrimitiveDrawingData<internal::line_vertex_format>>(max_line_vertices, max_line_indices);
            internal::_lines_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::line_vertex_format>>(max_line_vertices, max_line_indices);
            internal::_triangle_drawing_data = std::make_unique<PrimitiveDrawingData<internal::triangle_vertex_format>>(max_triangle_vertices, max_triangle_indices);
            internal::_triangle_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::triangle_vertex_format>>(max_triangle_vertices, max_triangle_indices);

            s32 max_texture_units = 0;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
            
            s32 max_images = std::min(max_texture_units, internal::_max_texture_units);
            s32 size_vertex_buffer = max_images * 4;
            s32 size_index_buffer = max_images * 4 * 3;

            // Image Drawing Data
            internal::_image_drawing_data = std::make_unique<TextureDrawingData<internal::image_vertex_format>>(max_images);
            internal::_image_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::triangle_vertex_format>>(size_vertex_buffer, size_index_buffer);

            s32 white = 0xFFFFFFFF;
            internal::_white_texture_image_id = create_image_item(1, 1, 4, (u8*)&white);
            s32 black = 0xFF000000;
            internal::_black_texture_image_id = create_image_item(1, 1, 4, (u8*)&black);

            // Font Drawing Data
            internal::_font_drawing_data = std::make_unique<TextureDrawingData<internal::image_vertex_format>>(max_images);

            return true;
        }

        void terminate()
        {
            internal::_font_drawing_data->release();
            internal::_image_drawing_data->release();
            internal::_image_stroke_drawing_data->release();
            internal::_triangle_drawing_data->release();
            internal::_triangle_stroke_drawing_data->release();
            internal::_lines_drawing_data->release();
            internal::_lines_stroke_drawing_data->release();
            internal::_points_drawing_data->release();
            internal::_points_stroke_drawing_data->release();
        }

        void begin()
        {
            internal::_points_drawing_data->reset();
            internal::_points_stroke_drawing_data->reset();
            internal::_lines_drawing_data->reset();
            internal::_lines_stroke_drawing_data->reset();
            internal::_triangle_drawing_data->reset();
            internal::_triangle_stroke_drawing_data->reset();
            internal::_image_drawing_data->reset();
            internal::_image_stroke_drawing_data->reset();
            internal::_font_drawing_data->reset();

            glBindFramebuffer(GL_FRAMEBUFFER, internal::_render_fbo);

            // Clear the background to black
            glViewport(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Reset to the user clear color
            glm::vec4 bg_color = internal::convert_color(internal::_bg_color);
            glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);

            if (internal::_scissor_enable)
            {
                glEnable(GL_SCISSOR_TEST);
                glScissor(
                    std::clamp(internal::_scissor_x, 0, internal::_frame_buffer_width),
                    std::clamp(internal::_scissor_y, 0, internal::_frame_buffer_height),
                    std::clamp(internal::_scissor_width, internal::_min_frame_buffer_width, internal::_frame_buffer_width),
                    std::clamp(internal::_scissor_height, internal::_min_frame_buffer_height, internal::_frame_buffer_height));
            }
            else
            {
                glDisable(GL_SCISSOR_TEST);
            }

            glClear(GL_COLOR_BUFFER_BIT);
        }

        void render()
        {
            f32 w = static_cast<f32>(internal::_frame_buffer_width);
            f32 h = static_cast<f32>(internal::_frame_buffer_height);

            glm::mat4 p = glm::ortho(0.0f, w, 0.0f, h, -100.0f, 100.0f);
            glm::mat4 v = glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 vp = p * v;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if(internal::_font_drawing_data->batch_count() > 0)
            {
                glUseProgram(internal::_font_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(internal::_font_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

                // We draw all images first this 
                // An effect of this will be that filled shapes are drawn on top of images ( a z-index might be in order here )
                // This however will make sure inner-stroke is possible
                internal::draw_images(internal::_font_drawing_data, internal::_font_shader_program);

                glUseProgram(0);
            }

            if (internal::_image_drawing_data->batch_count() > 0)
            {
                glUseProgram(internal::_image_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(internal::_image_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

                // We draw all images first this 
                // An effect of this will be that filled shapes are drawn on top of images ( a z-index might be in order here )
                // This however will make sure inner-stroke is possible
                internal::draw_images(internal::_image_drawing_data, internal::_image_shader_program);

                glUseProgram(0);
            }

            if (internal::_points_drawing_data->batch_count() > 0
                || internal::_lines_drawing_data->batch_count() > 0
                || internal::_triangle_drawing_data->batch_count() > 0)
            {
                glUseProgram(internal::_color_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(internal::_color_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

                // Points
                internal::draw_points(internal::_points_drawing_data);
                internal::draw_points(internal::_points_stroke_drawing_data);

                // Lines
                internal::draw_lines(internal::_lines_drawing_data);
                internal::draw_lines(internal::_lines_stroke_drawing_data);

                // Triangles

                // We draw the triangles first and afterwards the stroke
                // This only has to happen for the triangles because of the inner stroke capabilities
                // If we would draw the triangles last no inner stroke would be visible
                internal::draw_triangles(internal::_triangle_drawing_data);
                internal::draw_triangles(internal::_triangle_stroke_drawing_data);

                internal::draw_triangles(internal::_image_stroke_drawing_data);

                glUseProgram(0);
            }

            glBindFramebuffer(GL_READ_FRAMEBUFFER, internal::_render_fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, 0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void end()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void push_fill_color(const glm::vec4& color)
        {
            internal::_fill_color = internal::convert_color(color);
        }

        void push_fill_enable(bool enable)
        {
            internal::_fill_enable = enable;
        }

        void push_stroke_width(f32 w)
        {
            internal::_stroke_width = w;
        }

        void push_stroke_color(const glm::vec4& color)
        {
            internal::_stroke_color = internal::convert_color(color);
        }

        void push_stroke_enable(bool enable)
        {
            internal::_stroke_enable = enable;
        }

        void push_inner_stroke_width(f32 w)
        {
            internal::_inner_stroke_width = w;
        }

        void push_inner_stroke_color(const glm::vec4& color)
        {
            internal::_inner_stroke_color = internal::convert_color(color);
        }

        void push_inner_stroke_enable(bool enable)
        {
            internal::_inner_stroke_enable = enable;
        }

        void push_tint_color(const glm::vec4& color)
        {
            internal::_tint_color = internal::convert_color(color);
        }

        void push_tint_enable(bool enable)
        {
            internal::_tint_enable = enable;
        }

        void push_scissor(s32 x, s32 y, s32 width, s32 height)
        {
            internal::_scissor_x = x;
            internal::_scissor_y = y;
            internal::_scissor_width = width;
            internal::_scissor_height = height;
        }

        void push_scissor_enable(bool enable)
        {
            internal::_scissor_enable = enable;
        }

        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data)
        {
            return create_image_item(width, height, channels, data, ImageFilterType::NEAREST, ImageWrapType::REPEAT);
        }

        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data, ImageFilterType filter_type, ImageWrapType wrap_type)
        {
            GLint format = GL_INVALID_VALUE;
            GLint usage = GL_INVALID_VALUE;

            switch (channels)
            {
            case 1:
                format = GL_R8;
                usage = GL_RED;
                break;
            case 2:
                format = GL_RG8;
                usage = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                usage = GL_RGBA;
                break;
            case 4:
                format = GL_RGBA;
                usage = GL_RGBA;
                break;
            default:
                assert(false);
            }

            GLint filter = GL_INVALID_VALUE;

            switch(filter_type)
            {
                case ImageFilterType::NEAREST: filter = GL_NEAREST; break;
                case ImageFilterType::LINEAR: filter = GL_LINEAR; break;
                default:
                    assert(false);
            }

            GLint wrap = GL_INVALID_VALUE;

            switch(wrap_type)
            {
                case ImageWrapType::CLAMP_TO_EDGE: wrap = GL_CLAMP_TO_EDGE; break;
                case ImageWrapType::REPEAT: wrap = GL_REPEAT; break;
                default:
                    assert(false);
            }

            u32 texture_id;

            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

            glTexImage2D(
                GL_TEXTURE_2D,						// What (target)
                0,									// Mip-map level
                format,								// Internal format
                static_cast<GLsizei>(width),		// Width
                static_cast<GLsizei>(height),		// Height
                0,									// Border
                usage,								// Format (how to use)
                GL_UNSIGNED_BYTE,					// Type   (how to interpret)
                data);								// Data

            glBindTexture(GL_TEXTURE_2D, 0);

            return texture_id;
        }

        void update_image_item(u32 id, f32 x, f32 y, f32 width, f32 height, s32 channels, u8* data)
        {
            GLint format = GL_INVALID_VALUE;

            switch (channels)
            {
            case 1:
                format = GL_R8;
                break;
            case 2:
                format = GL_RG8;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                assert(false);
            }

            glBindTexture(GL_TEXTURE_2D, id);
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                static_cast<GLint>(x),
                static_cast<GLint>(y),
                static_cast<GLsizei>(width),
                static_cast<GLsizei>(height),
                format,
                GL_UNSIGNED_BYTE,
                data);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void submit_render_item(TopologyType topology, const RenderItem& item)
        {
            if (internal::_stroke_enable == false && internal::_fill_enable == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 fill_color = internal::convert_color(internal::_fill_enable ? internal::_fill_color : internal::_bg_color);

            switch (topology)
            {
            case TopologyType::POINTS:
                internal::_points_drawing_data->append(item, fill_color, transform::active_world());
                break;
            case TopologyType::LINES:
                internal::_lines_drawing_data->append(item, fill_color, transform::active_world());
                break;
            case TopologyType::TRIANGLES:
                internal::_triangle_drawing_data->append(item, fill_color, transform::active_world());
                break;

            default:
                log::critical("Invalid topology type!");
                break;
            }
        }

        void submit_stroke_render_item(TopologyType topology, const RenderItem& item, bool outer)
        {
            if (internal::_stroke_enable == false && internal::_fill_enable == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 stroke_color = outer ? internal::convert_color(internal::_stroke_color) : internal::convert_color(internal::_inner_stroke_color);

            switch (topology)
            {
            case TopologyType::POINTS:
                internal::_points_stroke_drawing_data->append(item, stroke_color, transform::active_world());
                break;
            case TopologyType::LINES:
                internal::_lines_stroke_drawing_data->append(item, stroke_color, transform::active_world());
                break;
            case TopologyType::TRIANGLES:
                internal::_triangle_stroke_drawing_data->append(item, stroke_color, transform::active_world());
                break;

            default:
                log::critical("Invalid topology type!");
                break;
            }
        }

        void submit_font_item(const ImageItem& item)
        {
            glm::vec4 fill_color = internal::convert_color(internal::_fill_color);

            internal::_font_drawing_data->append(item, fill_color, transform::active_world());
        }

        void submit_image_item(const ImageItem& item)
        {
            glm::vec4 tint_color = internal::convert_color(internal::_tint_color);

            internal::_image_drawing_data->append(item, tint_color, transform::active_world());
        }

        void submit_stroke_image_item(const RenderItem& item, bool outer)
        {
            glm::vec4 stroke_color = outer ? internal::convert_color(internal::_stroke_color) : internal::convert_color(internal::_inner_stroke_color);

            internal::_image_stroke_drawing_data->append(item, stroke_color, transform::active_world());
        }

        void clear_color(f32 r, f32 g, f32 b, f32 a)
        {
            internal::_bg_color = internal::convert_color(glm::vec4(r, g, b, a));
            glClearColor(r, g, b, a);
        }

        void clear(u32 flags)
        {           
            glClear(flags);
        }

        bool fill_enabled()
        {
            return internal::_fill_enable;
        }
        
        bool stroke_enabled()
        {
            return internal::_stroke_enable;
        }

        bool inner_stroke_enabled()
        {
            return internal::_inner_stroke_enable;
        }

        f32 stroke_width()
        {
            return internal::_stroke_width;
        }

        f32 inner_stroke_width()
        {
            return internal::_inner_stroke_width;
        }
        
        bool tint_enabled()
        {
            return internal::_tint_enable;
        }

        bool scissor_enabled()
        {
            return internal::_scissor_enable;
        }

        glm::vec4 fill()
        {
            return internal::convert_color(internal::_fill_color);
        }
        
        glm::vec4 stroke()
        {
            return internal::convert_color(internal::_stroke_color);
        }

        glm::vec4 inner_stroke()
        {
            return internal::convert_color(internal::_inner_stroke_color);
        }
        
        glm::vec4 tint()
        {
            return internal::convert_color(internal::_tint_color);
        }
        
        ScissorRect scissor()
        {
            return { internal::_scissor_x, internal::_scissor_y, internal::_scissor_width, internal::_scissor_height };
        }
    }
}