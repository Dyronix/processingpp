#include "render/render_shaders.h"

#include "util/log.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            namespace internal
            {
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
            }

            const char* const unlit_color_vertex_shader_code()
            {
                const char* const unlit_vs_source =
                    "#version 460 core												            \n\
                    layout (location = 0) in vec4 a_position;						            \n\
                    layout (location = 1) in vec4 a_color;							            \n\
                    layout (location = 2) uniform mat4 u_worldviewproj;				            \n\
                    layout (location = 3) uniform bool u_wireframe;                             \n\
                    layout (location = 4) uniform vec4 u_wireframe_color;                       \n\
                    out vec4 v_color;												            \n\
                                                                                                \n\
                    void main()														            \n\
                    {																            \n\
                        v_color = u_wireframe ? u_wireframe_color : a_color;	                \n\
                        gl_Position = u_worldviewproj * a_position, 1.0;			            \n\
                    }";

                    return unlit_vs_source;
            }
            const char* const unlit_color_pixel_shader_code()
            {
                const char* const unlit_fs_source =
                    "#version 460 core												            \n\
                    in vec4 v_color;												            \n\
                    out vec4 frag_color;											            \n\
                                                                                                \n\
                    void main()														            \n\
                    {																            \n\
                        frag_color = v_color;										            \n\
                    }";

                    return unlit_fs_source;
            }

            const char* const unlit_texture_vertex_shader_code()
            {
                const char* const unlit_texture_vs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                   \n\
                    layout (location = 1) in vec2 a_texture;                                    \n\
                    layout (location = 2) in vec4 a_tint_color;                                 \n\
                    layout (location = 3) in float a_texture_idx;                               \n\
                    layout (location = 4) uniform mat4 u_worldviewproj;                         \n\
                    layout (location = 5) uniform bool u_wireframe;                             \n\
                    layout (location = 6) uniform vec4 u_wireframe_color;                       \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec2 v_texture;                                                         \n\
                    out float v_texture_idx;                                                    \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {						                                                    \n\
                        v_tint_color = u_wireframe ? u_wireframe_color : a_tint_color;	        \n\
                        v_texture = a_texture;                                                  \n\
                        v_texture_idx = a_texture_idx;                                          \n\
                        gl_Position = u_worldviewproj * vec4(a_position, 1.0);                  \n\
                    }";

                return unlit_texture_vs_source;
            }
            const char* const unlit_texture_pixel_shader_code()
            {
                const char* const unlit_texture_fs_source =
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

                return unlit_texture_fs_source;
            }

            const char* const unlit_font_vertex_shader_code()
            {
                const char* const unlit_font_vs_source =
                    "#version 460 core                                                                          \n\
                                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                                   \n\
                    layout (location = 1) in vec2 a_texture;                                                    \n\
                    layout (location = 2) in vec4 a_tint_color;                                                 \n\
                    layout (location = 3) in float a_texture_idx;                                               \n\
                    layout (location = 4) uniform mat4 u_worldviewproj;                                         \n\
                    layout (location = 5) uniform bool u_wireframe;                                             \n\
                    layout (location = 6) uniform vec4 u_wireframe_color;                                       \n\
                                                                                                                \n\
                    out vec4 v_tint_color;                                                                      \n\
                    out vec2 v_texture;                                                                         \n\
                    out float v_texture_idx;                                                                    \n\
                                                                                                                \n\
                    void main()                                                                                 \n\
                    {						                                                                    \n\
                        v_tint_color = u_wireframe ? u_wireframe_color : a_tint_color;                          \n\
                        v_texture = a_texture;                                                                  \n\
                        v_texture_idx = a_texture_idx;                                                          \n\
                        gl_Position = u_worldviewproj * vec4(a_position, 1.0);                                  \n\
                    }";

                    return unlit_font_vs_source;
            }
            const char* const unlit_font_pixel_shader_code()
            {
                const char* const unlit_font_fs_source =
                    "#version 460 core                                                                          \n\
                                                                                                                \n\
                    layout (binding = 0) uniform sampler2D s_image[8];                                          \n\
                                                                                                                \n\
                    in vec4 v_tint_color;                                                                       \n\
                    in vec2 v_texture;                                                                          \n\
                    in float v_texture_idx;                                                                     \n\
                                                                                                                \n\
                    out vec4 frag_color;                                                                        \n\
                                                                                                                \n\
                    void main()                                                                                 \n\
                    {                                                                                           \n\
                        int idx = int(v_texture_idx);                                                           \n\
                        vec4 color = vec4(v_tint_color.rgb, texture(s_image[idx], v_texture).r);                \n\
                        frag_color = color;                                                                     \n\
                    }";

                    return unlit_font_fs_source;
            }
        
            u32 create_shader_program(const char* vs_source, const char* fs_source)
            {
                GLuint vert_shader = 0;
                GLuint frag_shader = 0;

                u32 shader_program = glCreateProgram();

                GLboolean res = internal::compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile vertex shader");
                    return -1;
                }

                res = internal::compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile fragment shader");
                    return -1;
                }

                glAttachShader(shader_program, vert_shader);
                glAttachShader(shader_program, frag_shader);

                if (!internal::link_program(shader_program))
                {
                    glDeleteShader(vert_shader);
                    glDeleteShader(frag_shader);
                    glDeleteProgram(shader_program);
                    log::error("Renderer failed to link shader program");
                    return -1;
                }

                glDeleteShader(vert_shader);
                glDeleteShader(frag_shader);

                return shader_program;
            }
        }
    }
}