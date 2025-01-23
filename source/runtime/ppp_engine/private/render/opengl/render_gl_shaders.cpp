#include "render/render_shaders.h"

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

                // uniforms
                class shader_uniform_manager
                {
                public:
                    shader_uniform_manager(u32 shader_program_id)
                        : m_shader_program_id(shader_program_id)
                    {
                        if (m_shader_program_id == 0)
                        {
                            log::error("Invalid shader program id (0).");
                        }
                    }

                    template<typename T>
                    void push(const std::string& name, const T& value)
                    {
                        if (m_deferred_uniforms.find(name) != std::cend(m_deferred_uniforms))
                        {
                            m_deferred_uniforms.at(name) = shader_uniform{ name, value };
                        }
                        else
                        {
                            m_deferred_uniforms.emplace(name, shader_uniform{ name, value });
                        }
                    }

                    template<typename T>
                    void push_array(const std::string& name, u64 count, const T* value)
                    {
                        if (m_deferred_uniforms.find(name) != std::cend(m_deferred_uniforms))
                        {
                            m_deferred_uniforms.at(name) = shader_uniform{ name, value, count };
                        }
                        else
                        {
                            m_deferred_uniforms.emplace(name, shader_uniform{ name, value, count });
                        }
                    }

                    void apply()
                    {
                        assert(m_shader_program_id != 0);

                        shader_guard guard(m_shader_program_id); // Automatically binds the shader program

                        for (const auto& pair : m_deferred_uniforms)
                        {
                            apply_uniform(pair.second);
                        }

                        m_deferred_uniforms.clear();
                    }

                private:
                    class shader_guard
                    {
                    public:
                        shader_guard(u32 shader_program_id)
                            : m_previous_program(0)
                            , m_needs_rebind(false)
                        {
                            // Get the currently bound shader program
                            GL_CALL(glGetIntegerv(GL_CURRENT_PROGRAM, &m_previous_program));

                            // Check if we need to bind the given shader program
                            if (static_cast<u32>(m_previous_program) != shader_program_id)
                            {
                                m_needs_rebind = true;
                                GL_CALL(glUseProgram(shader_program_id)); // Bind the new shader
                            }
                        }

                        ~shader_guard()
                        {
                            if (m_needs_rebind)
                            {
                                GL_CALL(glUseProgram(m_previous_program));  // Rebind the previous shader
                            }
                        }

                    private:
                        s32 m_previous_program;  // Store the previously bound shader program
                        bool m_needs_rebind;     // Flag to determine if we need to rebind the old shader
                    };

                    class shader_uniform
                    {
                    public:
                        template<typename T>
                        shader_uniform(const std::string& name, const T& value, u64 count = 1)
                            : m_name(name)
                            , m_type_id(type_tag<T>::value())
                            , m_count(count)
                            , m_is_array(false)
                        {
                            // Allocate the memory needed to store the value (using byte array)
                            m_data.resize(sizeof(value) * count);

                            std::memcpy(m_data.data(), &value, sizeof(value) * count);
                        }

                        template<typename T>
                        shader_uniform(const std::string& name, const T* value, u64 count = 1)
                            : m_name(name)
                            , m_type_id(type_tag<T*>::value())
                            , m_count(count)
                            , m_is_array(true)
                        {
                            // Allocate the memory needed to store the value (using byte array)
                            m_data.resize(sizeof(value) * count);

                            std::memcpy(m_data.data(), value, sizeof(value) * count);
                        }

                        const std::string& name() const 
                        {
                            return m_name; 
                        }

                        const object_type_tag& type_id() const 
                        {
                            return m_type_id; 
                        }

                        u64 count() const
                        {
                            return m_count;
                        }

                        bool is_array() const
                        {
                            return m_is_array;
                        }

                        template <typename T>
                        T get_value() const
                        {
                            assert(m_type_id == type_tag<T>::value());
                            assert(!m_is_array && "Value is an array please use shader_uniform::get_value_ptr");

                            T result;
                            std::memcpy(&result, m_data.data(), sizeof(T));
                            return result;
                        }

                        template <typename T>
                        const T* get_value_ptr() const
                        {
                            assert(m_type_id == type_tag<T*>::value());
                            assert(m_is_array && "Value is not an array please use shader_uniform::get_value");

                            return reinterpret_cast<const T*>(m_data.data());
                        }

                    private:
                        std::string m_name;          // Name of the uniform
                        object_type_tag m_type_id;   // Type identifier using type_tag
                        std::vector<u8> m_data;      // Binary data for the value
                        u64 m_count;                 // Amount of elements in the array of elements
                        bool m_is_array;             // Is the uniform storing an array of elements
                    };

                private:
                    u32 find_and_cache_uniform_location(const std::string& uniform_name)
                    {
                        // Check if the uniform location is already cached
                        if (m_shader_uniform_locations.find(uniform_name) == m_shader_uniform_locations.end())
                        {
                            // Get uniform location from OpenGL
                            GL_CALL(s32 location = glGetUniformLocation(m_shader_program_id, uniform_name.c_str()));
                            m_shader_uniform_locations[uniform_name] = location;
                        }

                        return m_shader_uniform_locations[uniform_name];
                    }

                    bool is_uniform_location_valid(s32 location)
                    {
                        return location != static_cast<u32>(-1);
                    }

                    void apply_uniform(const shader_uniform& uniform)
                    {
                        s32 location = find_and_cache_uniform_location(uniform.name());

                        if (!is_uniform_location_valid(location))
                        {
                            log::warn("Uniform '{}' not found in shader program {}.", uniform.name(), m_shader_program_id);
                            return;
                        }

                        if (uniform.is_array())
                        {
                            if (uniform.type_id() == type_tag<s32*>::value())
                            {
                                set_uniform(location, uniform.count(), uniform.get_value_ptr<s32>());
                            }
                            else if (uniform.type_id() == type_tag<u32*>::value())
                            {
                                const u32* data = uniform.get_value_ptr<u32>();
                                set_uniform(location, uniform.count(), uniform.get_value_ptr<u32>());
                            }
                            else if (uniform.type_id() == type_tag<f32*>::value())
                            {
                                const f32* data = uniform.get_value_ptr<f32>();
                                set_uniform(location, uniform.count(), uniform.get_value_ptr<f32>());
                            }
                        }
                        else
                        {
                            if (uniform.type_id() == type_tag<bool>::value())
                            {
                                set_uniform(location, uniform.get_value<bool>());
                            }
                            else if (uniform.type_id() == type_tag<s32>::value())
                            {
                                set_uniform(location, uniform.get_value<s32>());
                            }
                            else if (uniform.type_id() == type_tag<u32>::value())
                            {
                                set_uniform(location, uniform.get_value<u32>());
                            }
                            else if (uniform.type_id() == type_tag<f32>::value())
                            {
                                set_uniform(location, uniform.get_value<f32>());
                            }
                            else if (uniform.type_id() == type_tag<glm::vec2>::value())
                            {
                                set_uniform(location, uniform.get_value<glm::vec2>());
                            }
                            else if (uniform.type_id() == type_tag<glm::vec3>::value())
                            {
                                set_uniform(location, uniform.get_value<glm::vec3>());
                            }
                            else if (uniform.type_id() == type_tag<glm::vec4>::value())
                            {
                                set_uniform(location, uniform.get_value<glm::vec4>());
                            }
                            else if (uniform.type_id() == type_tag<glm::mat2>::value())
                            {
                                set_uniform(location, uniform.get_value<glm::mat2>());
                            }
                            else if (uniform.type_id() == type_tag<glm::mat3>::value())
                            {
                                set_uniform(location, uniform.get_value<glm::mat3>());
                            }
                            else if (uniform.type_id() == type_tag<glm::mat4>::value())
                            {
                                set_uniform(location, uniform.get_value<glm::mat4>());
                            }
                            else
                            {
                                log::warn("Unsupported uniform type for '{}' in shader program {}.", uniform.name(), m_shader_program_id);
                            }
                        }
                    }

                private:
                    void set_uniform(s32 location, bool value)
                    {
                        set_uniform(location, value ? GL_TRUE : GL_FALSE);
                    }

                    void set_uniform(s32 location, s32 value)
                    {
                        glUniform1i(location, value);
                    }

                    void set_uniform(s32 location, u32 value)
                    {
                        glUniform1ui(location, value);
                    }

                    void set_uniform(s32 location, f32 value)
                    {
                        glUniform1f(location, value);
                    }

                    void set_uniform(s32 location, const glm::vec2& value)
                    {
                        GL_CALL(glUniform2fv(location, 1, glm::value_ptr(value)));
                    }

                    void set_uniform(s32 location, const glm::vec3& value)
                    {
                        GL_CALL(glUniform3fv(location, 1, glm::value_ptr(value)));
                    }

                    void set_uniform(s32 location, const glm::vec4& value)
                    {
                        GL_CALL(glUniform4fv(location, 1, glm::value_ptr(value)));
                    }

                    void set_uniform(s32 location, const glm::mat2& value)
                    {
                        GL_CALL(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value)));
                    }

                    void set_uniform(s32 location, const glm::mat3& value)
                    {
                        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
                    }

                    void set_uniform(s32 location, const glm::mat4& value)
                    {
                        GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
                    }

                    void set_uniform(s32 location, u64 count, const s32* value)
                    {
                        GL_CALL(glUniform1iv(location, count, value));
                    }

                    void set_uniform(s32 location, u64 count, const u32* value)
                    {
                        GL_CALL(glUniform1uiv(location, count, value));
                    }

                    void set_uniform(s32 location, u64 count, const f32* value)
                    {
                        GL_CALL(glUniform1fv(location, count, value));
                    }

                private:
                    std::unordered_map<std::string, shader_uniform> m_deferred_uniforms;
                    std::unordered_map<std::string, u32> m_shader_uniform_locations;
                    u32 m_shader_program_id;
                };

                std::unordered_map<u32, shader_uniform_manager> _shader_uniform_managers;
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

            void push_uniform(u32 shader_program_id, const std::string& uniform_name, bool value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager { shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, s32 value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, f32 value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, const glm::vec2& value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, const glm::vec3& value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, const glm::vec4& value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, const glm::mat2& value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, const glm::mat3& value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }
            void push_uniform(u32 shader_program_id, const std::string& uniform_name, const glm::mat4& value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push(uniform_name, value);
            }

            void push_uniform_array(u32 shader_program_id, const std::string& uniform_name, u64 count, const s32* value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push_array(uniform_name, count, value);
            }
            void push_uniform_array(u32 shader_program_id, const std::string& uniform_name, u64 count, const u32* value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push_array(uniform_name, count, value);
            }
            void push_uniform_array(u32 shader_program_id, const std::string& uniform_name, u64 count, const f32* value)
            {
                if (internal::_shader_uniform_managers.find(shader_program_id) == std::cend(internal::_shader_uniform_managers))
                {
                    internal::_shader_uniform_managers.emplace(shader_program_id, internal::shader_uniform_manager{ shader_program_id });
                }

                internal::_shader_uniform_managers.at(shader_program_id).push_array(uniform_name, count, value);
            }

            void apply_uniforms(u32 shader_program_id)
            {
                assert(internal::_shader_uniform_managers.find(shader_program_id) != std::cend(internal::_shader_uniform_managers));

                internal::_shader_uniform_managers.at(shader_program_id).apply();
            }
        }
    }
}