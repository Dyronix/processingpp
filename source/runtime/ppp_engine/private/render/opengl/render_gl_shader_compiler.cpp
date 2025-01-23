#include "render/render_shader_compiler.h"

#include "render/opengl/render_gl_error.h"

#include "util/log.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp >

#include <unordered_map>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            namespace internal
            {
                // shader compilation
                bool compile_shader(GLuint* shader, GLenum type, const GLchar* source)
                {
                    GLint status;

                    if (!source)
                    {
                        log::error("Failed to compile empty shader");
                        return false;
                    }

                    GL_CALL(*shader = glCreateShader(type));
                    GL_CALL(glShaderSource(*shader, 1, &source, nullptr));

                    GL_CALL(glCompileShader(*shader));

                    #if defined(_DEBUG)
                    GLint log_length = 0;
                    GL_CALL(glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &log_length));
                    if (log_length > 1)
                    {
                        GLchar* log = static_cast<GLchar*>(malloc(log_length));
                        GL_CALL(glGetShaderInfoLog(*shader, log_length, &log_length, log));
                        if (log)
                            log::error("Program compile log: {}", log);
                        free(log);
                    }
                    #endif

                    GL_CALL(glGetShaderiv(*shader, GL_COMPILE_STATUS, &status));
                    if (status == 0)
                    {
                        GL_CALL(glDeleteShader(*shader));
                        return false;
                    }

                    return true;
                }

                bool link_program(GLuint program)
                {
                    GLint status;

                    GL_CALL(glLinkProgram(program));

                    #if defined(_DEBUG)
                    GLint logLength = 0;
                    GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength));
                    if (logLength > 1)
                    {
                        GLchar* log = static_cast<GLchar*>(malloc(logLength));
                        GL_CALL(glGetProgramInfoLog(program, logLength, &logLength, log));
                        if (log)
                            log::error("Program link log: {}", log);
                        free(log);
                    }
                    #endif

                    GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &status));
                    return status != 0;
                }
            }

            u32 create_shader_program(const char* vs_source, const char* fs_source)
            {
                GLuint vert_shader = 0;
                GLuint frag_shader = 0;

                GL_CALL(u32 shader_program = glCreateProgram());

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

                GL_CALL(glAttachShader(shader_program, vert_shader));
                GL_CALL(glAttachShader(shader_program, frag_shader));

                if (!internal::link_program(shader_program))
                {
                    GL_CALL(glDeleteShader(vert_shader));
                    GL_CALL(glDeleteShader(frag_shader));
                    GL_CALL(glDeleteProgram(shader_program));
                    log::error("Renderer failed to link shader program");
                    return -1;
                }

                GL_CALL(glDeleteShader(vert_shader));
                GL_CALL(glDeleteShader(frag_shader));

                return shader_program;
            }
            u32 create_shader_program(const char* vs_source, const char* fs_source, const char* geometry_source)
            {
                GLuint vert_shader = 0;
                GLuint geom_shader = 0;
                GLuint frag_shader = 0;

                GL_CALL(u32 shader_program = glCreateProgram());

                GLboolean res = internal::compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile vertex shader");
                    return -1;
                }

                res = internal::compile_shader(&geom_shader, GL_GEOMETRY_SHADER, geometry_source);
                if (!res)
                {
                    log::error("Renderer failed to compile geometry shader");
                    return -1;
                }

                res = internal::compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_source);
                if (!res)
                {
                    log::error("Renderer failed to compile fragment shader");
                    return -1;
                }

                GL_CALL(glAttachShader(shader_program, vert_shader));
                GL_CALL(glAttachShader(shader_program, geom_shader));
                GL_CALL(glAttachShader(shader_program, frag_shader));

                if (!internal::link_program(shader_program))
                {
                    GL_CALL(glDeleteShader(vert_shader));
                    GL_CALL(glDeleteShader(geom_shader));
                    GL_CALL(glDeleteShader(frag_shader));
                    GL_CALL(glDeleteProgram(shader_program));
                    log::error("Renderer failed to link shader program");
                    return -1;
                }

                GL_CALL(glDeleteShader(vert_shader));
                GL_CALL(glDeleteShader(geom_shader));
                GL_CALL(glDeleteShader(frag_shader));

                return shader_program;
            }
        }
    }
}