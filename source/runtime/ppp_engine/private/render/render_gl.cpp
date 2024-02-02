#include "render/render.h"
#include "util/log.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>

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
                out  = (conversions::f32_to_uint8(color.x)) << 24;
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

            constexpr s32 _min_frame_buffer_width = 32;
            constexpr s32 _min_frame_buffer_height = 32;

            s32 _frame_buffer_width = -1;
            s32 _frame_buffer_height = -1;

            s32 _scissor_x = -1;
            s32 _scissor_y = -1;
            s32 _scissor_width = -1;
            s32 _scissor_height = -1;
            bool _scissor_enable = false;

            s32 _fill_color = 0xFFFFFFFF;
            bool _fill_enable = true;
            s32 _stroke_color = 0xFF000000;
            bool _stroke_enable = false;
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

            constexpr s32 _max_points = 10'000;
            constexpr s32 _max_lines = 10'000;
            constexpr s32 _max_triangles = 10'000;
            constexpr s32 _max_images = 8;

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
                    layout (location = 3) in float a_texture_idx;                                \n\
                    layout (location = 4) uniform mat4 u_worldviewproj;                         \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec2 v_texture;                                                         \n\
                    out float v_texture_idx;                                                     \n\
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
                    in float v_texture_idx;                                                      \n\
                                                                                                \n\
                    out vec4 frag_color;                                                        \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {                                                                           \n\
                        int idx = int(v_texture_idx);                                            \n\
                        vec4 color = texture(s_image[idx], v_texture) * v_tint_color; \n\
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

            u32	_points_vao = 0;
            u32	_points_vbo = 0;
            u32 _points_ebo = 0;
            point_vertex_format _point_vertices[_max_points];
            Index _point_indices[_max_points];
            u32 _nr_points = 0;
            u32 _nr_points_vertices = 0;
            u32 _nr_points_indices = 0;

            void setup_points(const RenderItem& item, const glm::vec4& fill_color)
            {
                memcpy(&internal::_point_indices[internal::_nr_points_indices], item.indices.data(), sizeof(Index) * item.indices.size());
                // For each index that was added we need to offset it with the amount of indices that are already within the array.
                for (s32 i = 0; i < item.indices.size(); ++i)
                {
                    internal::_point_indices[internal::_nr_points_indices + i] += internal::_nr_points_vertices;
                }
                internal::_nr_points_indices += item.indices.size();

                internal::point_vertex_format fmt;
                for (const auto& v : item.vertices)
                {
                    fmt.position = v.position;
                    fmt.color = fill_color;
                    internal::_point_vertices[internal::_nr_points_vertices] = fmt;
                    ++internal::_nr_points_vertices;
                }
                internal::_nr_points += item.indices.size();
            }

            u32	_lines_vao = 0;
            u32	_lines_vbo = 0;
            u32 _lines_ebo = 0;
            line_vertex_format _line_vertices[_max_lines];
            Index _line_indices[_max_lines * 2];
            u32 _nr_lines = 0;
            u32 _nr_lines_vertices = 0;
            u32 _nr_lines_indices = 0;

            void setup_lines(const RenderItem& item, const glm::vec4& fill_color)
            {
                memcpy(&internal::_line_indices[internal::_nr_lines_indices], item.indices.data(), sizeof(Index) * item.indices.size());
                // For each index that was added we need to offset it with the amount of indices that are already within the array.
                for (s32 i = 0; i < item.indices.size(); ++i)
                {
                    internal::_line_indices[internal::_nr_lines_indices + i] += internal::_nr_lines_vertices;
                }
                internal::_nr_lines_indices += item.indices.size();

                internal::line_vertex_format fmt;
                for (const auto& v : item.vertices)
                {
                    fmt.position = v.position;
                    fmt.color = fill_color;
                    internal::_line_vertices[internal::_nr_lines_vertices] = fmt;
                    ++internal::_nr_lines_vertices;
                }

                internal::_nr_lines += item.indices.size() / 2;
            }

            u32	_triangles_vao = 0;
            u32	_triangles_vbo = 0;
            u32 _triangles_ebo = 0;
            triangle_vertex_format _triangle_vertices[_max_triangles];
            Index _triangle_indices[_max_triangles * 3];
            u32 _nr_triangles = 0;
            u32 _nr_triangles_vertices = 0;
            u32 _nr_triangles_indices = 0;

            void setup_triangles(const RenderItem& item, const glm::vec4& fill_color)
            {
                memcpy(&internal::_triangle_indices[internal::_nr_triangles_indices], item.indices.data(), sizeof(Index) * item.indices.size());
                // For each index that was added we need to offset it with the amount of indices that are already within the array.
                for (s32 i = 0; i < item.indices.size(); ++i)
                {
                    internal::_triangle_indices[internal::_nr_triangles_indices + i] += internal::_nr_triangles_vertices;
                }
                internal::_nr_triangles_indices += item.indices.size();

                internal::triangle_vertex_format fmt;
                for (const auto& v : item.vertices)
                {
                    fmt.position = v.position;
                    fmt.color = fill_color;
                    internal::_triangle_vertices[internal::_nr_triangles_vertices] = fmt;
                    ++internal::_nr_triangles_vertices;
                }

                internal::_nr_triangles += item.indices.size() / 3;
            }

            u32	_images_vao = 0;
            u32	_images_vbo = 0;
            u32 _images_ebo = 0;
            image_vertex_format _image_vertices[_max_images * 4];
            Index _image_indices[_max_images * 3 * 4];
            s32 _image_sampler_ids[_max_images];
            std::unordered_map<u32, s32> _image_ids;
            u32 _nr_images = 0;
            u32 _nr_images_vertices = 0;
            u32 _nr_images_indices = 0;

            void setup_image(const ImageItem& item, const glm::vec4& tint_color)
            {
                bool existing_image = internal::_image_ids.find(item.image_id) != std::cend(internal::_image_ids);

                memcpy(&internal::_image_indices[internal::_nr_images_indices], item.indices.data(), sizeof(Index) * item.indices.size());
                // For each index that was added we need to offset it with the amount of indices that are already within the array.
                for (s32 i = 0; i < item.indices.size(); ++i)
                {
                    internal::_image_indices[internal::_nr_images_indices + i] += internal::_nr_images_vertices;
                }
                internal::_nr_images_indices += item.indices.size();

                internal::image_vertex_format fmt;
                for (const auto& v : item.vertices)
                {
                    fmt.position = v.position;
                    fmt.texcoord = v.texcoord;
                    fmt.color = internal::_tint_enable ? tint_color : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    fmt.texture_idx = existing_image ? (f32)internal::_image_ids.at(item.image_id) : (f32)internal::_nr_images;
                    internal::_image_vertices[internal::_nr_images_vertices] = fmt;
                    ++internal::_nr_images_vertices;
                }

                if(!existing_image)
                {
                    internal::_image_sampler_ids[internal::_nr_images] = internal::_nr_images;
                    internal::_image_ids.insert(std::make_pair(item.image_id, internal::_nr_images));

                    ++internal::_nr_images;
                }
            }

            void initialize_points_buffer()
            {
                // Allocate VAO
                glGenVertexArrays(1, &_points_vao);
                glBindVertexArray(_points_vao);

                // Allocate VBO
                glGenBuffers(1, &_points_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, _points_vbo);
                const size_t size_vbo = sizeof(_point_vertices);
                glBufferData(GL_ARRAY_BUFFER, size_vbo, nullptr, GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                auto pos_offset = offsetof(point_vertex_format, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(point_vertex_format), reinterpret_cast<void*>(pos_offset));

                glEnableVertexAttribArray(1);
                auto col_offset = offsetof(point_vertex_format, color);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(point_vertex_format), reinterpret_cast<void*>(col_offset));                

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Allocate EBO
                glGenBuffers(1, &_points_ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _points_ebo);
                const size_t size_ebo = sizeof(Index) * _max_points;
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(0);
            }
            void initialize_lines_buffer()
            {
                // Allocate VAO
                glGenVertexArrays(1, &_lines_vao);
                glBindVertexArray(_lines_vao);

                // Allocate VBO
                glGenBuffers(1, &_lines_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, _lines_vbo);
                const size_t size_vbo = sizeof(_line_vertices);
                glBufferData(GL_ARRAY_BUFFER, size_vbo, nullptr, GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                auto pos_offset = offsetof(line_vertex_format, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(line_vertex_format), reinterpret_cast<void*>(pos_offset));

                glEnableVertexAttribArray(1);
                auto col_offset = offsetof(line_vertex_format, color);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(line_vertex_format), reinterpret_cast<void*>(col_offset));

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Allocate EBO
                glGenBuffers(1, &_lines_ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _lines_ebo);
                const size_t size_ebo = sizeof(Index) * (_max_lines * 2);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(0);
            }
            void initialize_triangles_buffer()
            {
                // Allocate VAO
                glGenVertexArrays(1, &_triangles_vao);
                glBindVertexArray(_triangles_vao);

                // Allocate VBO
                glGenBuffers(1, &_triangles_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, _triangles_vbo);
                const size_t size_vbo = sizeof(_triangle_vertices);
                glBufferData(GL_ARRAY_BUFFER, size_vbo, nullptr, GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                auto pos_offset = offsetof(triangle_vertex_format, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(triangle_vertex_format), reinterpret_cast<void*>(pos_offset));

                glEnableVertexAttribArray(1);
                auto col_offset = offsetof(triangle_vertex_format, color);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(triangle_vertex_format), reinterpret_cast<void*>(col_offset));

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Allocate EBO
                glGenBuffers(1, &_triangles_ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _triangles_ebo);
                const size_t size_ebo = sizeof(Index) * (_max_triangles * 3);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(0);
            }
            void initialize_images_buffer()
            {
                // Allocate VAO
                glGenVertexArrays(1, &_images_vao);
                glBindVertexArray(_images_vao);

                // Allocate VBO
                glGenBuffers(1, &_images_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, _images_vbo);
                const size_t size_vbo = sizeof(_image_vertices);
                glBufferData(GL_ARRAY_BUFFER, size_vbo, nullptr, GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                auto pos_offset = offsetof(image_vertex_format, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(image_vertex_format), reinterpret_cast<void*>(pos_offset));

                glEnableVertexAttribArray(1);
                auto tc_offset = offsetof(image_vertex_format, texcoord);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(image_vertex_format), reinterpret_cast<void*>(tc_offset));

                glEnableVertexAttribArray(2);
                auto col_offset = offsetof(image_vertex_format, color);
                glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(image_vertex_format), reinterpret_cast<void*>(col_offset));

                glEnableVertexAttribArray(3);
                auto ti_offset = offsetof(image_vertex_format, texture_idx);
                glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(image_vertex_format), reinterpret_cast<void*>(ti_offset));

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Allocate EBO
                glGenBuffers(1, &_images_ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _images_ebo);
                const size_t size_ebo = sizeof(Index) * (_max_images * 3 * 4);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ebo, nullptr, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(0);
            }

            void terminate_points_buffer()
            {
                glDeleteBuffers(1, &_points_vbo);
                glDeleteBuffers(1, &_points_ebo);
                glDeleteVertexArrays(1, &_points_vao);
            }
            void terminate_lines_buffer()
            {
                glDeleteBuffers(1, &_lines_vbo);
                glDeleteBuffers(1, &_lines_ebo);
                glDeleteVertexArrays(1, &_lines_vao);
            }
            void terminate_triangles_buffer()
            {
                glDeleteBuffers(1, &_triangles_vbo);
                glDeleteBuffers(1, &_triangles_ebo);
                glDeleteVertexArrays(1, &_triangles_vao);
            }
            void terminate_images_buffer()
            {
                glDeleteBuffers(1, &_images_vbo);
                glDeleteBuffers(1, &_images_ebo);
                glDeleteVertexArrays(1, &_images_vao);
            }
        }

        bool initialize(s32 w, s32 h, void* user_data)
        {
            // glad: load all OpenGL function pointers
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
            internal::create_frame_buffer();

            internal::initialize_points_buffer();
            internal::initialize_lines_buffer();
            internal::initialize_triangles_buffer();
            internal::initialize_images_buffer();

            return true;
        }

        void terminate()
        {
            internal::terminate_images_buffer();
            internal::terminate_triangles_buffer();
            internal::terminate_lines_buffer();
            internal::terminate_points_buffer();
        }

        void begin()
        {
            internal::_nr_points = 0;
            internal::_nr_points_vertices = 0;
            internal::_nr_points_indices = 0;

            internal::_nr_lines = 0;
            internal::_nr_lines_vertices = 0;
            internal::_nr_lines_indices = 0;

            internal::_nr_triangles = 0;
            internal::_nr_triangles_vertices = 0;
            internal::_nr_triangles_indices = 0;

            internal::_nr_images = 0;
            internal::_nr_images_vertices = 0;
            internal::_nr_images_indices = 0;

            internal::_image_ids.clear();

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
            f32 w = static_cast<f32>(internal::_frame_buffer_width * 0.5f);
            f32 h = static_cast<f32>(internal::_frame_buffer_height * 0.5f);

            glm::mat4 p = glm::ortho(-w, w, -h, h, -100.0f, 100.0f);
            glm::mat4 v = glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 vp = p * v;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if (internal::_nr_points > 0 || internal::_nr_lines > 0 || internal::_nr_triangles > 0)
            {
                glUseProgram(internal::_color_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(internal::_color_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

                if (internal::_nr_points > 0)
                {
                    glBindVertexArray(internal::_points_vao);
                    glBindBuffer(GL_ARRAY_BUFFER, internal::_points_vbo);

                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(internal::point_vertex_format) * internal::_nr_points_vertices, &internal::_point_vertices[0]);
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Index) * internal::_nr_points_indices, &internal::_point_indices[0]);

                    glDrawElements(GL_POINTS, internal::_nr_points_indices, internal::index_type(), nullptr);

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }

                if (internal::_nr_lines > 0)
                {
                    #ifndef NDEBUG
                    if (internal::_nr_lines_indices % 2 != 0)
                    {
                        log::error("Trying to render invalid number of lines: {}", internal::_nr_lines_indices);
                        return;
                    }
                    #endif
                    glBindVertexArray(internal::_lines_vao);
                    glBindBuffer(GL_ARRAY_BUFFER, internal::_lines_vbo);

                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(internal::line_vertex_format) * internal::_nr_lines_vertices, &internal::_line_vertices[0]);
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Index) * internal::_nr_lines_indices, &internal::_line_indices[0]);

                    glDrawElements(GL_LINES, internal::_nr_lines_indices, internal::index_type(), nullptr);

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }

                if (internal::_nr_triangles > 0)
                {
                    #ifndef NDEBUG
                    if (internal::_nr_triangles_indices % 3 != 0)
                    {
                        log::error("Trying to render invalid number of triangles: {}", internal::_nr_triangles_indices);
                        return;
                    }
                    #endif
                    glBindVertexArray(internal::_triangles_vao);
                    glBindBuffer(GL_ARRAY_BUFFER, internal::_triangles_vbo);

                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(internal::triangle_vertex_format) * internal::_nr_triangles_vertices, &internal::_triangle_vertices[0]);
                    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Index) * internal::_nr_triangles_indices, &internal::_triangle_indices[0]);

                    glDrawElements(GL_TRIANGLES, internal::_nr_triangles_indices, internal::index_type(), nullptr);

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }

                glUseProgram(0);
            }

            if (internal::_nr_images > 0)
            {
                glUseProgram(internal::_image_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(internal::_image_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));
                u32 u_tex_samplers_loc = glGetUniformLocation(internal::_image_shader_program, "s_image");
                glUniform1iv(u_tex_samplers_loc, internal::_nr_images, &internal::_image_sampler_ids[0]);

                #ifndef NDEBUG
                if (internal::_nr_images_indices % 3 != 0)
                {
                    log::error("Trying to render invalid number of triangles: {}", internal::_nr_images_indices);
                    return;
                }
                #endif
                glBindVertexArray(internal::_images_vao);
                glBindBuffer(GL_ARRAY_BUFFER, internal::_images_vbo);

                s32 i = 0;
                s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                for (const auto& pair : internal::_image_ids)
                {
                    u32 id = pair.first;
                    s32 idx = pair.second;

                    glActiveTexture(GL_TEXTURE0 + (offset * i));
                    glBindTexture(GL_TEXTURE_2D, id);

                    ++i;
                }

                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(internal::image_vertex_format) * internal::_nr_images_vertices, &internal::_image_vertices[0]);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Index) * internal::_nr_images_indices, &internal::_image_indices[0]);

                glDrawElements(GL_TRIANGLES, internal::_nr_images_indices, internal::index_type(), nullptr);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);

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

        void push_stroke_color(const glm::vec4& color)
        {
            internal::_stroke_color = internal::convert_color(color);
        }

        void push_stroke_enable(bool enable)
        {
            internal::_stroke_enable = enable;
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

            u32 texture_id;

            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(
                GL_TEXTURE_2D,						// What (target)
                0,									// Mip-map level
                format,								// Internal format
                width,							    // Width
                height,							    // Height
                0,									// Border
                usage,								// Format (how to use)
                GL_UNSIGNED_BYTE,					// Type   (how to interpret)
                data);								// Data

            return texture_id;
        }

        void submit_render_item(TopologyType topology, const RenderItem& item)
        {
            if (internal::_stroke_enable == false && internal::_fill_enable == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 fill_color = internal::convert_color(internal::_fill_color);

            switch (topology)
            {
            case TopologyType::POINTS: internal::setup_points(item, fill_color); break;
            case TopologyType::LINES: internal::setup_lines(item, fill_color); break;
            case TopologyType::TRIANGLES: internal::setup_triangles(item, fill_color); break;

            default:
                log::critical("Invalid topology type!");
                break;
            }
        }

        void submit_image_item(const ImageItem& item)
        {
            glm::vec4 tint_color = internal::convert_color(internal::_fill_color);

            internal::setup_image(item, tint_color);
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
    }
}