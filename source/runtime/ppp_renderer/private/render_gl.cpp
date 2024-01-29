#include "render.h"
#include "log.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            s32 _frame_buffer_width = -1;
            s32 _frame_buffer_height = -1;

            s32 _viewport_width = -1;
            s32 _viewport_height = -1;

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

                #if defined(DEBUG)
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

            u32	_points_vao = 0;
            u32	_points_vbo = 0;
            point_vertex_format _points[_max_points];
            u32 _nr_points = 0;

            u32	_lines_vao = 0;
            u32	_lines_vbo = 0;
            line_vertex_format _lines[_max_lines];
            u32 _nr_lines = 0;

            u32	_triangles_vao = 0;
            u32	_triangles_vbo = 0;
            triangle_vertex_format _triangles[_max_triangles];
            u32 _nr_triangles = 0;

            void initialize_points_buffer()
            {
                glGenVertexArrays(1, &_points_vao);
                glBindVertexArray(_points_vao);

                // Allocate VBO
                glGenBuffers(1, &_points_vbo);

                // Array buffer contains the attribute data
                glBindBuffer(GL_ARRAY_BUFFER, _points_vbo);

                // Allocate into VBO
                const size_t size = sizeof(_points);
                glBufferData(GL_ARRAY_BUFFER, size, &_points[0], GL_STREAM_DRAW);

                auto pos_offset = offsetof(point_vertex_format, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(point_vertex_format), reinterpret_cast<void*>(pos_offset));
                glEnableVertexAttribArray(0);

                auto col_offset = offsetof(point_vertex_format, color);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(point_vertex_format), reinterpret_cast<void*>(col_offset));
                glEnableVertexAttribArray(1);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
            void initialize_lines_buffer()
            {
                glGenVertexArrays(1, &_lines_vao);
                glBindVertexArray(_lines_vao);

                // Allocate VBO
                glGenBuffers(1, &_lines_vbo);

                // Array buffer contains the attribute data
                glBindBuffer(GL_ARRAY_BUFFER, _lines_vbo);

                // Allocate into VBO
                const size_t size = sizeof(_lines);
                glBufferData(GL_ARRAY_BUFFER, size, &_lines[0], GL_STREAM_DRAW);

                auto pos_offset = offsetof(line_vertex_format, position);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(line_vertex_format), reinterpret_cast<void*>(pos_offset));
                glEnableVertexAttribArray(0);

                auto col_offset = offsetof(line_vertex_format, color);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(line_vertex_format), reinterpret_cast<void*>(col_offset));
                glEnableVertexAttribArray(1);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
            void initialize_triangles_buffer()
            {
                glGenVertexArrays(1, &_triangles_vao);
                glBindVertexArray(_triangles_vao);

                // Allocate VBO
                glGenBuffers(1, &_triangles_vbo);

                // Array buffer contains the attribute data
                glBindBuffer(GL_ARRAY_BUFFER, _triangles_vbo);

                // Allocate into VBO
                const size_t size = sizeof(_triangles);
                glBufferData(GL_ARRAY_BUFFER, size, &_triangles[0], GL_STREAM_DRAW);

                auto pos_offset = offsetof(triangle_vertex_format, position);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(triangle_vertex_format), reinterpret_cast<void*>(pos_offset));
                glEnableVertexAttribArray(0);

                auto col_offset = offsetof(triangle_vertex_format, color);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(triangle_vertex_format), reinterpret_cast<void*>(col_offset));
                glEnableVertexAttribArray(1);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            void terminate_points_buffer()
            {
                glDeleteVertexArrays(1, &_points_vao);
                glDeleteBuffers(1, &_points_vbo);
            }
            void terminate_lines_buffer()
            {
                glDeleteVertexArrays(1, &_lines_vao);
                glDeleteBuffers(1, &_lines_vbo);
            }
            void terminate_triangles_buffer()
            {
                glDeleteVertexArrays(1, &_triangles_vao);
                glDeleteBuffers(1, &_triangles_vbo);
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

            internal::compile_color_shader_program();
            internal::create_frame_buffer();

            internal::initialize_points_buffer();
            internal::initialize_lines_buffer();
            internal::initialize_triangles_buffer();

            return true;
        }

        void terminate()
        {
            internal::terminate_triangles_buffer();
            internal::terminate_lines_buffer();
            internal::terminate_points_buffer();
        }

        void begin()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, internal::_render_fbo);
        }

        void render()
        {
            auto w = internal::_frame_buffer_width * 0.5f;
            auto h = internal::_frame_buffer_height * 0.5f;

            glm::mat4 p = glm::ortho(-w, w, -h, h, -100.0f, 100.0f);
            glm::mat4 v = glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 vp = p * v;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glUseProgram(internal::_color_shader_program);
            glUniformMatrix4fv(2, 1, false, value_ptr(vp));

            if (internal::_nr_points > 0)
            {
                glBindVertexArray(internal::_points_vao);
                glBindBuffer(GL_ARRAY_BUFFER, internal::_points_vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(internal::point_vertex_format) * internal::_nr_points, &internal::_points[0], GL_DYNAMIC_DRAW);
                glDrawArrays(GL_POINTS, 0, internal::_nr_points);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            if (internal::_nr_lines > 0)
            {
#ifndef NDEBUG
                //if (internal::_nr_lines % 2 != 0)
                //{
                //    log::error("Trying to render invalid number of lines: {}", internal::_nr_lines);
                //    return;
                //}
#endif

                glBindVertexArray(internal::_lines_vao);
                glBindBuffer(GL_ARRAY_BUFFER, internal::_lines_vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(internal::line_vertex_format) * internal::_nr_lines, &internal::_lines[0], GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINES, 0, internal::_nr_lines);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            if (internal::_nr_triangles > 0)
            {
#ifndef NDEBUG
                //if (internal::_nr_triangles % 3 != 0)
                //{
                //    log::error("Trying to render invalid number of triangles: {}", internal::_nr_triangles);
                //    return;
                //}
#endif

                glBindVertexArray(internal::_triangles_vao);
                glBindBuffer(GL_ARRAY_BUFFER, internal::_triangles_vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(internal::triangle_vertex_format) * internal::_nr_triangles, &internal::_triangles[0], GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLES, 0, internal::_nr_triangles);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            glUseProgram(0);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, internal::_render_fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, 0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void end()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void submit_render_item(TopologyType topology, const RenderItem& item)
        {
            switch (topology)
            {
            case TopologyType::POINTS:
            {
                internal::point_vertex_format fmt;
                for (const auto& v : item.vertices)
                {
                    fmt.position = v;
                    fmt.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    internal::_points[internal::_nr_points] = fmt;
                    ++internal::_nr_points;
                }
            }
                break;
            case TopologyType::LINES:
            {
                internal::line_vertex_format fmt;
                for (const auto& v : item.vertices)
                {
                    fmt.position = v;
                    fmt.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    internal::_lines[internal::_nr_lines] = fmt;
                    ++internal::_nr_lines;
                }
            }
                break;
            case TopologyType::TRIANGLES:
            {
                internal::triangle_vertex_format fmt;
                for (const auto& v : item.vertices)
                {
                    fmt.position = v;
                    fmt.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    internal::_triangles[internal::_nr_triangles] = fmt;
                    ++internal::_nr_triangles;
                }
            }
                break;

            default:
                log::critical("Invalid topology type!");
                break;
            }
        }

        void viewport(s32 x, s32 y, s32 width, s32 height)
        {
            internal::_viewport_width = width;
            internal::_viewport_height = height;

            glViewport(x, y, width, height);
        }

        void clear_color(f32 r, f32 g, f32 b, f32 a)
        {
            glClearColor(r, g, b, a);
        }

        void clear(u32 flags)
        {
            glClear(flags);

            internal::_nr_points = 0;
            internal::_nr_lines = 0;
            internal::_nr_triangles = 0;
        }
    }
}