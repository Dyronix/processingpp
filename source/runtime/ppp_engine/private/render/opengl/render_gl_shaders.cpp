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

            const char* const unlit_color_vertex_shader_code()
            {
                const char* const unlit_vs_source =
                    "#version 460 core												                \n\
                    layout (location = 0) in vec3 a_position;						                \n\
                    layout (location = 1) in vec4 a_color;							                \n\
                                                                                                    \n\
                    uniform mat4 u_view_proj;                                                       \n\
                    uniform bool u_wireframe;                                                       \n\
                    uniform vec4 u_wireframe_color;                                                 \n\
                                                                                                    \n\
                    out vec4 v_color;												                \n\
                                                                                                    \n\
                    void main()														                \n\
                    {																                \n\
                        v_color = u_wireframe ? u_wireframe_color : a_color;	                    \n\
                        gl_Position = u_view_proj * vec4(a_position, 1.0);  	                    \n\
                    }";

                return unlit_vs_source;
            }
            const char* const unlit_color_pixel_shader_code()
            {
                const char* const unlit_fs_source =
                    "#version 460 core												                \n\
                    in vec4 v_color;												                \n\
                    out vec4 frag_color;											                \n\
                                                                                                    \n\
                    void main()														                \n\
                    {																                \n\
                        frag_color = v_color;										                \n\
                    }";

                return unlit_fs_source;
            }

            const char* const instance_unlit_color_vertex_shader_code()
            {
                const char* const unlit_vs_source =
                    "#version 460 core												                \n\
                    layout (location = 0) in vec3 a_position;						                \n\
                                                                                                    \n\
                    // per instance data                                                            \n\
                    layout (location = 1) in mat4 a_inst_mat_model;							        \n\
                    layout (location = 5) in vec4 a_inst_color;							            \n\
                                                                                                    \n\
                    uniform mat4 u_view_proj;                                                       \n\
                    uniform bool u_wireframe;                                                       \n\
                    uniform vec4 u_wireframe_color;                                                 \n\
                                                                                                    \n\
                    out vec4 v_color;												                \n\
                                                                                                    \n\
                    void main()														                \n\
                    {																                \n\
                        v_color = u_wireframe ? u_wireframe_color : a_inst_color;                   \n\
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);       \n\
                    }";

                return unlit_vs_source;
            }

            const char* const unlit_texture_vertex_shader_code()
            {
                const char* const unlit_texture_vs_source =
                    "#version 460 core                                                              \n\
                                                                                                    \n\
                    layout (location = 0) in vec3 a_position;                                       \n\
                    layout (location = 1) in vec2 a_texture;                                        \n\
                    layout (location = 2) in vec4 a_tint_color;                                     \n\
                    layout (location = 3) in int  a_material_idx;                                    \n\
                                                                                                    \n\
                    uniform mat4 u_view_proj;                                                       \n\
                    uniform bool u_wireframe;                                                       \n\
                    uniform vec4 u_wireframe_color;                                                 \n\
                                                                                                    \n\
                    out vec4 v_tint_color;                                                          \n\
                    out vec2 v_texture;                                                             \n\
                    flat out int v_material_idx;                                                     \n\
                                                                                                    \n\
                    void main()                                                                     \n\
                    {						                                                        \n\
                        v_tint_color = u_wireframe ? u_wireframe_color : a_tint_color;	            \n\
                        v_texture = a_texture;                                                      \n\
                        v_material_idx = a_material_idx;                                              \n\
                        gl_Position = u_view_proj * vec4(a_position, 1.0);                          \n\
                    }";

                return unlit_texture_vs_source;
            }
            const char* const unlit_texture_pixel_shader_code()
            {
                const char* const unlit_texture_fs_source =
                    "#version 460 core                                                              \n\
                                                                                                    \n\
                    struct material_attributes                                                      \n\
                    {                                                                               \n\
                        int sampler_indices[8];                                                    \n\
                        int sampler_count;                                                          \n\
                        vec4 ambient_color;                                                         \n\
                        vec4 diffuse_color;                                                         \n\
                    };                                                                              \n\
                                                                                                    \n\
                    layout (std430, binding = 1) buffer MaterialBuffer                              \n\
                    {                                                                               \n\
                        material_attributes materials[];                                            \n\
                    };                                                                              \n\
                                                                                                    \n\
                    layout (binding = 0) uniform sampler2D s_image[8];                             \n\
                                                                                                    \n\
                    in vec4 v_tint_color;                                                           \n\
                    in vec2 v_texture;                                                              \n\
                    flat in int v_material_idx;                                                     \n\
                                                                                                    \n\
                    out vec4 frag_color;                                                            \n\
                                                                                                    \n\
                    void main()                                                                     \n\
                    {                                                                               \n\
                        vec4 color = texture(s_image[materials[v_material_idx].sampler_indices[0]], v_texture) * v_tint_color;     \n\
                        frag_color = color;                                                         \n\
                    }";

                return unlit_texture_fs_source;
            }

            const char* const instance_unlit_texture_vertex_shader_code()
            {
                const char* const unlit_texture_vs_source =
                    "#version 460 core                                                              \n\
                                                                                                    \n\
                    layout (location = 0) in vec3 a_position;                                       \n\
                    layout (location = 1) in vec2 a_texture;                                        \n\
                                                                                                    \n\
                    // per instance data                                                            \n\
                    layout (location = 2) in int  a_inst_material_idx;                              \n\
                    layout (location = 3) in mat4 a_inst_mat_model;							        \n\
                    layout (location = 7) in vec4 a_inst_color;							            \n\
                                                                                                    \n\
                    uniform mat4 u_view_proj;                                                       \n\
                    uniform bool u_wireframe;                                                       \n\
                    uniform vec4 u_wireframe_color;                                                 \n\
                                                                                                    \n\
                    out vec4 v_tint_color;                                                          \n\
                    out vec2 v_texture;                                                             \n\
                    flat out int v_material_idx;                                                    \n\
                                                                                                    \n\
                    void main()                                                                     \n\
                    {						                                                        \n\
                        v_tint_color = u_wireframe ? u_wireframe_color : a_inst_color;	            \n\
                        v_texture = a_texture;                                                      \n\
                        v_material_idx = a_inst_material_idx;                                       \n\
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);       \n\
                    }";

                return unlit_texture_vs_source;
            }

            const char* const unlit_font_vertex_shader_code()
            {
                const char* const unlit_font_vs_source =
                    "#version 460 core                                                                          \n\
                                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                                   \n\
                    layout (location = 1) in vec2 a_texture;                                                    \n\
                    layout (location = 2) in vec4 a_tint_color;                                                 \n\
                    layout (location = 3) in int a_texture_idx;                                                 \n\
                                                                                                                \n\
                    uniform mat4 u_view_proj;                                                                   \n\
                    uniform bool u_wireframe;                                                                   \n\
                    uniform vec4 u_wireframe_color;                                                             \n\
                                                                                                                \n\
                    out vec4 v_tint_color;                                                                      \n\
                    out vec2 v_texture;                                                                         \n\
                    flat out int v_texture_idx;                                                                      \n\
                                                                                                                \n\
                    void main()                                                                                 \n\
                    {						                                                                    \n\
                        v_tint_color = u_wireframe ? u_wireframe_color : a_tint_color;                          \n\
                        v_texture = a_texture;                                                                  \n\
                        v_texture_idx = a_texture_idx;                                                          \n\
                        gl_Position = u_view_proj * vec4(a_position, 1.0);                                      \n\
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
                    flat in int v_texture_idx;                                                                       \n\
                                                                                                                \n\
                    out vec4 frag_color;                                                                        \n\
                                                                                                                \n\
                    void main()                                                                                 \n\
                    {                                                                                           \n\
                        vec4 color = vec4(v_tint_color.rgb, texture(s_image[v_texture_idx], v_texture).r);      \n\
                        frag_color = color;                                                                     \n\
                    }";

                return unlit_font_fs_source;
            }

            const char* const unlit_normal_vertex_shader_code()
            {
                const char* const unlit_normal_vs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                   \n\
                    layout (location = 1) in vec3 a_normal;                                     \n\
                    layout (location = 2) in vec4 a_tint_color;                                 \n\
                                                                                                \n\
                    uniform mat4 u_view_proj;                                                   \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec3 v_normal;                                                          \n\
                    flat out int v_texture_idx;                                                 \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {						                                                    \n\
                        v_tint_color = a_tint_color;	                                        \n\
                        v_normal = a_normal;                                                    \n\
                        gl_Position = u_view_proj * vec4(a_position, 1.0);                      \n\
                    }";

                return unlit_normal_vs_source;
            }
            const char* const unlit_normal_pixel_shader_code()
            {
                const char* const unlit_normal_fs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    in vec4 v_tint_color;                                                       \n\
                    in vec3 v_normal;                                                           \n\
                                                                                                \n\
                    out vec4 frag_color;                                                        \n\
                                                                                                \n\
                    uniform float u_brightness;                                                 \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {                                                                           \n\
                        float brightness = u_brightness > 0.0 ? u_brightness : 0.8;             \n\
                        vec3 color = ((v_normal * 0.5) + 0.5) * brightness;                     \n\
                        frag_color = vec4(color, 1.0);                                          \n\
                    }";

                return unlit_normal_fs_source;
            }

            const char* const instance_unlit_normal_vertex_shader_code()
            {
                const char* const unlit_normal_vs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                   \n\
                    layout (location = 1) in vec3 a_normal;                                     \n\
                                                                                                \n\
                    // per instance data                                                        \n\
                    layout (location = 2) in mat4 a_inst_mat_model;							    \n\
                    layout (location = 6) in vec4 a_inst_color;							        \n\
                                                                                                \n\
                    uniform mat4 u_view_proj;                                                   \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec3 v_normal;                                                          \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {						                                                    \n\
                        v_tint_color = a_inst_color;	                                        \n\
                        v_normal = a_normal;                                                    \n\
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);   \n\
                    }";

                return unlit_normal_vs_source;
            }

            const char* const specular_vertex_shader_code()
            {
                const char* const unlit_specular_vs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                   \n\
                    layout (location = 1) in vec3 a_normal;                                     \n\
                    layout (location = 2) in vec4 a_tint_color;                                 \n\
                                                                                                \n\
                    uniform mat4 u_view_proj;                                                   \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec3 v_position;                                                        \n\
                    out vec3 v_normal;                                                          \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {						                                                    \n\
                        v_tint_color = a_tint_color;	                                        \n\
                        v_position = a_position;                                                \n\
                        v_normal = a_normal;                                                    \n\
                                                                                                \n\
                        gl_Position = u_view_proj * vec4(a_position, 1.0);                      \n\
                    }";

                return unlit_specular_vs_source;
            }
            const char* const specular_pixel_shader_code()
            {
                const char* const unlit_normal_fs_source =
                    "#version 460 core                                                                          \n\
                                                                                                                \n\
                    in vec4 v_tint_color;                                                                       \n\
                    in vec3 v_position;                                                                         \n\
                    in vec3 v_normal;                                                                           \n\
                                                                                                                \n\
                    out vec4 frag_color;                                                                        \n\
                                                                                                                \n\
                    uniform float u_ambient_strength;                                                           \n\
                    uniform vec3 u_light_position;                                                              \n\
                    uniform vec3 u_light_color;                                                                 \n\
                    uniform vec3 u_specular_color;                                                              \n\
                    uniform float u_specular_strength;                                                          \n\
                    uniform int u_specular_power;                                                               \n\
                    uniform vec3 u_view_position;                                                               \n\
                                                                                                                \n\
                    void main()                                                                                 \n\
                    {                                                                                           \n\
                        // ambient                                                                              \n\
                        vec3 ambient = u_ambient_strength * u_light_color;                                      \n\
                                                                                                                \n\
                        // diffuse                                                                              \n\
                        vec3 norm = normalize(v_normal);                                                        \n\
                        vec3 light_direction = normalize(u_light_position - v_position);                        \n\
                        float diff = max(dot(norm, light_direction), 0.0);                                      \n\
                        vec3 diffuse = diff * u_light_color;                                                    \n\
                                                                                                                \n\
                        // specular                                                                             \n\
                        vec3 view_direction = normalize(u_view_position - v_position);                          \n\
                        vec3 reflect_dir = reflect(-light_direction, norm);                                     \n\
                        float spec = pow(max(dot(view_direction, reflect_dir), 0.0), u_specular_power);         \n\
                        vec3 specular = u_specular_strength * spec * u_specular_color;                          \n\
                                                                                                                \n\
                        vec4 result = vec4((ambient + diffuse + specular), 1.0) * v_tint_color;                 \n\
                        frag_color = result;                                                                    \n\
                    }";

                return unlit_normal_fs_source;
            }

            const char* const instance_specular_vertex_shader_code()
            {
                const char* const unlit_specular_vs_source =
                    "#version 460 core                                                          \n\
                                                                                                \n\
                    layout (location = 0) in vec3 a_position;                                   \n\
                    layout (location = 1) in vec3 a_normal;                                     \n\
                                                                                                \n\
                    // per instance data                                                        \n\
                    layout (location = 2) in mat4 a_inst_mat_model;							    \n\
                    layout (location = 6) in vec4 a_inst_color;							        \n\
                                                                                                \n\
                    uniform mat4 u_view_proj;                                                   \n\
                                                                                                \n\
                    out vec4 v_tint_color;                                                      \n\
                    out vec3 v_position;                                                        \n\
                    out vec3 v_normal;                                                          \n\
                                                                                                \n\
                    void main()                                                                 \n\
                    {						                                                    \n\
                        v_tint_color = a_inst_color;	                                        \n\
                        v_position = a_position;                                                \n\
                        v_normal = a_normal;                                                    \n\
                                                                                                \n\
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);   \n\
                    }";

                return unlit_specular_vs_source;
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