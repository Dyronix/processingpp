#include "render/render_shader_uniform_manager.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "util/log.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp >

#include <unordered_map>
#include <optional>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
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
                void push(string::string_id name, const T& value)
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
                void push_array(string::string_id name, u64 count, const T* value)
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

                    for (auto& pair : m_deferred_uniforms)
                    {
                        if (pair.second)
                        {
                            apply_uniform(*pair.second);

                            pair.second = {};
                        }
                    }
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
                        opengl::api::instance().get_integer_value(GL_CURRENT_PROGRAM, &m_previous_program);

                        // Check if we need to bind the given shader program
                        if (static_cast<u32>(m_previous_program) != shader_program_id)
                        {
                            m_needs_rebind = true;
                            opengl::api::instance().use_program(shader_program_id); // Bind the new shader
                        }
                    }

                    ~shader_guard()
                    {
                        if (m_needs_rebind)
                        {
                            opengl::api::instance().use_program(m_previous_program);  // Rebind the previous shader
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
                    shader_uniform(string::string_id name, const T& value, u64 count = 1)
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
                    shader_uniform(string::string_id name, const T* value, u64 count = 1)
                        : m_name(name)
                        , m_type_id(type_tag<T*>::value())
                        , m_count(count)
                        , m_is_array(true)
                    {
                        // Allocate the memory needed to store the value (using byte array)
                        m_data.resize(sizeof(value) * count);

                        std::memcpy(m_data.data(), value, sizeof(value) * count);
                    }

                    string::string_id name() const
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
                    string::string_id m_name;    // Name of the uniform
                    object_type_tag m_type_id;   // Type identifier using type_tag
                    std::vector<u8> m_data;      // Binary data for the value
                    u64 m_count;                 // Amount of elements in the array of elements
                    bool m_is_array;             // Is the uniform storing an array of elements
                };

            private:
                u32 find_and_cache_uniform_location(string::string_id uniform_name)
                {
                    // Check if the uniform location is already cached
                    if (m_shader_uniform_locations.find(uniform_name) == m_shader_uniform_locations.end())
                    {
                        // Get uniform location from OpenGL
                        s32 location = opengl::api::instance().get_uniform_location(m_shader_program_id, string::restore_sid(uniform_name).data());
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
                        log::warn("Uniform '{}' not found in shader program {}.", string::restore_sid(uniform.name()), m_shader_program_id);
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
                            log::warn("Unsupported uniform type for '{}' in shader program {}.", string::restore_sid(uniform.name()), m_shader_program_id);
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
                    opengl::api::instance().uniform_1i(location, value);
                }

                void set_uniform(s32 location, u32 value)
                {
                    opengl::api::instance().uniform_1ui(location, value);
                }

                void set_uniform(s32 location, f32 value)
                {
                    opengl::api::instance().uniform_1f(location, value);
                }

                void set_uniform(s32 location, const glm::vec2& value)
                {
                    opengl::api::instance().uniform_2fv(location, 1, glm::value_ptr(value));
                }

                void set_uniform(s32 location, const glm::vec3& value)
                {
                    opengl::api::instance().uniform_3fv(location, 1, glm::value_ptr(value));
                }

                void set_uniform(s32 location, const glm::vec4& value)
                {
                    opengl::api::instance().uniform_4fv(location, 1, glm::value_ptr(value));
                }

                void set_uniform(s32 location, const glm::mat2& value)
                {
                    opengl::api::instance().uniform_matrix_2fv(location, 1, GL_FALSE, glm::value_ptr(value));
                }

                void set_uniform(s32 location, const glm::mat3& value)
                {
                    opengl::api::instance().uniform_matrix_3fv(location, 1, GL_FALSE, glm::value_ptr(value));
                }

                void set_uniform(s32 location, const glm::mat4& value)
                {
                    opengl::api::instance().uniform_matrix_4fv(location, 1, GL_FALSE, glm::value_ptr(value));
                }

                void set_uniform(s32 location, u64 count, const s32* value)
                {
                    opengl::api::instance().uniform_1iv(location, count, value);
                }

                void set_uniform(s32 location, u64 count, const u32* value)
                {
                    opengl::api::instance().uniform_1uiv(location, count, value);
                }

                void set_uniform(s32 location, u64 count, const f32* value)
                {
                    opengl::api::instance().uniform_1fv(location, count, value);
                }

            private:
                std::unordered_map<string::string_id, std::optional<shader_uniform>> m_deferred_uniforms;
                std::unordered_map<string::string_id, u32> m_shader_uniform_locations;

                u32 m_shader_program_id;
            };

            struct context_uniform_manager
            {
                std::unordered_map<u32, shader_uniform_manager> shader_umgr;
            } g_umgr_ctx;

            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, bool value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, s32 value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, f32 value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::vec2& value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::vec3& value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::vec4& value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::mat2& value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::mat3& value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::mat4& value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push(uniform_name, value);
            }

            //-------------------------------------------------------------------------
            void push_uniform_array(u32 shader_program_id, string::string_id uniform_name, u64 count, const s32* value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push_array(uniform_name, count, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform_array(u32 shader_program_id, string::string_id uniform_name, u64 count, const u32* value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push_array(uniform_name, count, value);
            }
            //-------------------------------------------------------------------------
            void push_uniform_array(u32 shader_program_id, string::string_id uniform_name, u64 count, const f32* value)
            {
                if (g_umgr_ctx.shader_umgr.find(shader_program_id) == std::cend(g_umgr_ctx.shader_umgr))
                {
                    g_umgr_ctx.shader_umgr.emplace(shader_program_id, shader_uniform_manager{ shader_program_id });
                }

                g_umgr_ctx.shader_umgr.at(shader_program_id).push_array(uniform_name, count, value);
            }

            //-------------------------------------------------------------------------
            void apply_uniforms(u32 shader_program_id)
            {
                assert(g_umgr_ctx.shader_umgr.find(shader_program_id) != std::cend(g_umgr_ctx.shader_umgr));

                g_umgr_ctx.shader_umgr.at(shader_program_id).apply();
            }
        }
    }
}