#include "render/render_shader_library.h"

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            class shader_builder
            {
            public:
                //-------------------------------------------------------------------------
                shader_builder& set_version(s32 version)
                {
                    m_shader_code << "#version " << version << " core\n";
                    return *this;
                }

                //-------------------------------------------------------------------------
                shader_builder& add_attribute(const std::string& type, const std::string& name, s32 location)
                {
                    m_shader_code << "layout (location = " << location << ") in " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_input(const std::string& type, const std::string& name)
                {
                    m_shader_code << "in " << type << ";\n";
                }
                //-------------------------------------------------------------------------
                shader_builder& add_output(const std::string& type, const std::string& name)
                {
                    m_shader_code << "out " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_uniform(const std::string& type, const std::string& name)
                {
                    m_shader_code << "uniform " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_function(const std::string& function_code)
                {
                    m_shader_code << function_code << "\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& set_main_function_body(const std::string& main_code)
                {
                    m_shader_code << "void main() {\n" << main_code << "\n}\n";
                    return *this;
                }

                //-------------------------------------------------------------------------
                std::string build() const
                {
                    return m_shader_code.str();
                }

            private:
                std::ostringstream m_shader_code;
            };

            std::string unlit_color_vertex_shader_code()
            {
                shader_builder builder;

                return builder.set_version(460)
                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("vec4", "a_color", 1)
                    .add_uniform("mat4", "u_view_proj")
                    .add_uniform("bool", "u_wireframe")
                    .add_uniform("vec4", "u_wireframe_color")
                    .add_output("vec4", "v_color")
                    .set_main_function_body(R"(
                        v_color = u_wireframe ? u_wireframe_color : a_color;
                        gl_Position = u_view_proj * vec4(a_position, 1.0);
                    )").build();
            }
            std::string unlit_color_pixel_shader_code()
            {
                shader_builder builder;

                return builder.set_version(460)
                    .add_input("vec4", "v_color")
                    .add_output("vec4", "frag_color")
                    .set_main_function_body(R"(
                        frag_color = v_color;
                    )").build();
            }

            std::string instance_unlit_color_vertex_shader_code()
            {
                shader_builder builder;

                return builder.set_version(460)
                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("mat4", "a_inst_mat_model", 1)
                    .add_attribute("vec4", "a_inst_color", 5)
                    .add_uniform("mat4", "u_view_proj")
                    .add_uniform("bool", "u_wireframe")
                    .add_uniform("vec4", "u_wireframe_color")
                    .add_output("vec4", "v_color")
                    .set_main_function_body(R"(
                        v_color = u_wireframe ? u_wireframe_color : a_inst_color;
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
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
                        vec4 tex0 = texture(s_image[materials[v_material_idx].sampler_indices[0]], v_texture); \n\
                        vec4 tex1 = texture(s_image[materials[v_material_idx].sampler_indices[1]], v_texture); \n\
                        vec4 final_tex = mix(tex0, tex1, 0.2);                                                                  \n\
                        vec4 color = final_tex * v_tint_color;                      \n\
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
        }
    }
}