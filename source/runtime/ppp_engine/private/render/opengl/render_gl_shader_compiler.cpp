#include "render/render_shader_compiler.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

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

                    *shader = opengl::api::instance().create_shader(type);

                    opengl::api::instance().shader_source(*shader, 1, &source, nullptr);
                    opengl::api::instance().compile_shader(*shader);

                    #if defined(_DEBUG)
                    GLint log_length = 0;
                    opengl::api::instance().get_shader_integer_value(*shader, GL_INFO_LOG_LENGTH, &log_length);
                    if (log_length > 1)
                    {
                        GLchar* log = static_cast<GLchar*>(malloc(log_length));
                        opengl::api::instance().get_shader_info_log(*shader, log_length, &log_length, log);
                        if (log)
                            log::error("Program compile log: {}", log);
                        free(log);
                    }
                    #endif

                    opengl::api::instance().get_shader_integer_value(*shader, GL_COMPILE_STATUS, &status);
                    if (status == 0)
                    {
                        opengl::api::instance().delete_shader(*shader);
                        return false;
                    }

                    return true;
                }

                bool link_program(GLuint program)
                {
                    GLint status;

                    opengl::api::instance().link_program(program);

                    #if defined(_DEBUG)
                    GLint log_length = 0;
                    opengl::api::instance().get_program_integer_value(program, GL_INFO_LOG_LENGTH, &log_length);
                    if (log_length > 1)
                    {
                        GLchar* log = static_cast<GLchar*>(malloc(log_length));
                        opengl::api::instance().get_program_info_log(program, log_length, &log_length, log);
                        if (log)
                        {
                            log::error("Program link log: {}", log);
                        }
                        free(log);
                    }
                    #endif

                    opengl::api::instance().get_program_integer_value(program, GL_LINK_STATUS, &status);
                    return status != 0;
                }
            }

            u32 create_shader_program(std::string_view vs_source, std::string_view fs_source)
            {
                GLuint vert_shader = 0;
                GLuint frag_shader = 0;

                u32 shader_program = opengl::api::instance().create_program();

                GLboolean res = internal::compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_source.data());
                if (!res)
                {
                    log::error("Renderer failed to compile vertex shader");
                    return -1;
                }

                res = internal::compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_source.data());
                if (!res)
                {
                    log::error("Renderer failed to compile fragment shader");
                    return -1;
                }

                opengl::api::instance().attach_shader(shader_program, vert_shader);
                opengl::api::instance().attach_shader(shader_program, frag_shader);

                if (!internal::link_program(shader_program))
                {
                    opengl::api::instance().delete_shader(vert_shader);
                    opengl::api::instance().delete_shader(frag_shader);
                    opengl::api::instance().delete_program(shader_program);
                    log::error("Renderer failed to link shader program");
                    return -1;
                }

                opengl::api::instance().delete_shader(vert_shader);
                opengl::api::instance().delete_shader(frag_shader);

                return shader_program;
            }
            u32 create_shader_program(std::string_view vs_source, std::string_view fs_source, std::string_view geometry_source)
            {
                GLuint vert_shader = 0;
                GLuint geom_shader = 0;
                GLuint frag_shader = 0;

                u32 shader_program = opengl::api::instance().create_program();

                GLboolean res = internal::compile_shader(&vert_shader, GL_VERTEX_SHADER, vs_source.data());
                if (!res)
                {
                    log::error("Renderer failed to compile vertex shader");
                    return -1;
                }

                res = internal::compile_shader(&geom_shader, GL_GEOMETRY_SHADER, geometry_source.data());
                if (!res)
                {
                    log::error("Renderer failed to compile geometry shader");
                    return -1;
                }

                res = internal::compile_shader(&frag_shader, GL_FRAGMENT_SHADER, fs_source.data());
                if (!res)
                {
                    log::error("Renderer failed to compile fragment shader");
                    return -1;
                }

                opengl::api::instance().attach_shader(shader_program, vert_shader);
                opengl::api::instance().attach_shader(shader_program, geom_shader);
                opengl::api::instance().attach_shader(shader_program, frag_shader);

                if (!internal::link_program(shader_program))
                {
                    opengl::api::instance().delete_shader(vert_shader);
                    opengl::api::instance().delete_shader(geom_shader);
                    opengl::api::instance().delete_shader(frag_shader);
                    opengl::api::instance().delete_program(shader_program);
                    log::error("Renderer failed to link shader program");
                    return -1;
                }

                opengl::api::instance().delete_shader(vert_shader);
                opengl::api::instance().delete_shader(geom_shader);
                opengl::api::instance().delete_shader(frag_shader);

                return shader_program;
            }
        }
    }
}