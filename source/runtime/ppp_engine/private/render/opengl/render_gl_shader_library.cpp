#include "render/render_shader_library.h"
#include "render/render_features.h"

#include "util/string_ops.h"

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
                shader_builder& add_struct(const pool_string& name, const pool_vector<std::pair<pool_string, pool_string>>& members)
                {
                    m_shader_code << "struct " << name << "\n{\n";
                    for (const auto& member : members)
                    {
                        m_shader_code << "    " << member.first << " " << member.second << ";\n";
                    }
                    m_shader_code << "};\n\n";
                    return *this;
                }

                //-------------------------------------------------------------------------
                shader_builder& add_ssbo(const pool_string& name, const pool_string& struct_name, const pool_string& variable_name, s32 binding, bool is_array = false)
                {
                    m_shader_code << "layout (std430, binding = " << binding << ") buffer " << name << "\n{\n";
                    m_shader_code << "    " << struct_name << " " << variable_name;

                    if (is_array)
                    {
                        m_shader_code << "[];\n";
                    }
                    else
                    {
                        m_shader_code << ";\n";
                    }

                    m_shader_code << "};\n\n";
                    return *this;
                }

                //-------------------------------------------------------------------------
                shader_builder& add_attribute(const pool_string& type, const pool_string& name, s32 location)
                {
                    m_shader_code << "layout (location = " << location << ") in " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_input(const pool_string& type, const pool_string& name, bool is_flat = false)
                {
                    if(is_flat) m_shader_code << "flat ";
                    m_shader_code << "in " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_output(const pool_string& type, const pool_string& name, bool is_flat = false)
                {
                    if (is_flat) m_shader_code << "flat ";
                    m_shader_code << "out " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_uniform(const pool_string& type, const pool_string& name)
                {
                    m_shader_code << "uniform " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_uniform_array(const pool_string& type, const pool_string& name, s32 size)
                {
                    m_shader_code << "uniform " << type << " " << name << "[" << size << "]; \n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_function(const pool_string& function_code)
                {
                    m_shader_code << function_code << "\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& set_main_function_body(const pool_string& main_code)
                {
                    m_shader_code << "void main() {\n" << main_code << "\n}\n";
                    return *this;
                }

                //-------------------------------------------------------------------------
                pool_string build() const
                {
                    return m_shader_code.str();
                }

            private:
                pool_ostringstream m_shader_code;
            };

            pool_string unlit_color_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

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
            pool_string unlit_color_pixel_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_input("vec4", "v_color")
                    .add_output("vec4", "frag_color")
                    .set_main_function_body(R"(
                        frag_color = v_color;
                    )").build();
            }

            pool_string instance_unlit_color_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

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

            pool_string unlit_texture_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("vec2", "a_texture", 1)
                    .add_attribute("vec4", "a_tint_color", 2)
                    .add_attribute("int", "a_material_idx", 3)

                    .add_uniform("mat4", "u_view_proj")
                    .add_uniform("bool", "u_wireframe")
                    .add_uniform("vec4", "u_wireframe_color")

                    .add_output("vec4", "v_tint_color")
                    .add_output("vec2", "v_texture")
                    .add_output("int", "v_material_idx", true)

                    .set_main_function_body(R"(
                        v_tint_color = u_wireframe ? u_wireframe_color : a_tint_color;
                        v_texture = a_texture;                                        
                        v_material_idx = a_material_idx;                              
                        gl_Position = u_view_proj * vec4(a_position, 1.0);            
                    )").build();
            }

            pool_string unlit_texture_pixel_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_struct(
                        "material_attributes",
                        {
                            {"int", "sampler_indices[" + string::to_string<pool_string>(render::max_textures()) + "]"},
                            {"int", "sampler_count"},
                            {"vec4", "ambient_color"},
                            {"vec4", "diffuse_color"}
                        })
                    .add_ssbo("material_buffer", "material_attributes", "materials", 1, true)

                    .add_uniform_array("sampler2D", "s_images", render::max_textures())
                    
                    .add_input("vec4", "v_tint_color")
                    .add_input("vec2", "v_texture")
                    .add_input("int", "v_material_idx", true)
                    
                    .add_output("vec4", "frag_color")

                    .set_main_function_body(R"(
                        frag_color = texture(s_images[materials[v_material_idx].sampler_indices[0]], v_texture);
                    )").build();
            }

            pool_string instance_unlit_texture_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_attribute("vec3", "a_position", 0)                    
                    .add_attribute("vec2", "a_texture", 1)                     
                    .add_attribute("int", "a_inst_material_idx", 2)            
                    .add_attribute("mat4", "a_inst_mat_model", 3)              
                    .add_attribute("vec4", "a_inst_color", 7)            

                    .add_uniform("mat4", "u_view_proj")                 
                    .add_uniform("bool", "u_wireframe")                 
                    .add_uniform("vec4", "u_wireframe_color")           

                    .add_output("vec4", "v_tint_color")                 
                    .add_output("vec2", "v_texture")                    
                    .add_output("int", "v_material_idx", true)          

                    .set_main_function_body(R"(
                        v_tint_color = u_wireframe ? u_wireframe_color : a_inst_color;
                        v_texture = a_texture;
                        v_material_idx = a_inst_material_idx;
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
            }

            pool_string unlit_font_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("vec2", "a_texture", 1)
                    .add_attribute("vec4", "a_tint_color", 2)
                    .add_attribute("int", "a_texture_idx", 3)

                    .add_uniform("mat4", "u_view_proj")
                    .add_uniform("bool", "u_wireframe")
                    .add_uniform("vec4", "u_wireframe_color")

                    .add_output("vec4", "v_tint_color")
                    .add_output("vec2", "v_texture")
                    .add_output("int", "v_texture_idx", true)

                    .set_main_function_body(R"(
                        v_tint_color = u_wireframe ? u_wireframe_color : a_tint_color;
                        v_texture = a_texture;
                        v_texture_idx = a_texture_idx;
                        gl_Position = u_view_proj * vec4(a_position, 1.0);
                    )").build();
            }
            pool_string unlit_font_pixel_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_uniform_array("sampler2D", "s_images", render::max_textures())

                    .add_input("vec4", "v_tint_color")
                    .add_input("vec2", "v_texture")
                    .add_input("int", "v_texture_idx", true)

                    .add_output("vec4", "frag_color")

                    .set_main_function_body(R"(
                        vec4 color = vec4(v_tint_color.rgb, texture(s_images[v_texture_idx], v_texture).r); 
                        frag_color = color;
                    )").build();
            }

            pool_string unlit_normal_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("vec3", "a_normal", 1)
                    .add_attribute("vec4", "a_tint_color", 2)

                    .add_uniform("mat4", "u_view_proj")

                    .add_output("vec4", "v_tint_color")
                    .add_output("vec3", "v_normal")

                    .set_main_function_body(R"(
                        v_tint_color = a_tint_color;
                        v_normal = a_normal;
                        gl_Position = u_view_proj * vec4(a_position, 1.0);
                    )").build();
            }
            pool_string unlit_normal_pixel_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_input("vec4", "v_tint_color")
                    .add_input("vec3", "v_normal")

                    .add_output("vec4", "frag_color")

                    .add_uniform("float", "u_brightness")

                    .set_main_function_body(R"(
                        float brightness = u_brightness > 0.0 ? u_brightness : 0.8;
                        vec3 color = ((v_normal * 0.5) + 0.5) * brightness;
                        frag_color = vec4(color, 1.0);
                    )").build();
            }

            pool_string instance_unlit_normal_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("vec3", "a_normal", 1)
                    .add_attribute("mat4", "a_inst_mat_model", 2)
                    .add_attribute("vec4", "a_inst_color", 6)

                    .add_uniform("mat4", "u_view_proj")

                    .add_output("vec4", "v_tint_color")
                    .add_output("vec3", "v_normal")

                    .set_main_function_body(R"(
                        v_tint_color = a_inst_color;
                        v_normal = a_normal;
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
            }

            pool_string specular_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("vec3", "a_normal", 1)
                    .add_attribute("vec4", "a_tint_color", 2)

                    .add_uniform("mat4", "u_view_proj")

                    .add_output("vec4", "v_tint_color")
                    .add_output("vec3", "v_position")
                    .add_output("vec3", "v_normal")

                    .set_main_function_body(R"(
                        v_tint_color = a_tint_color;
                        v_position = a_position;  
                        v_normal = a_normal;
                        gl_Position = u_view_proj * vec4(a_position, 1.0);
                    )").build();
            }

            pool_string specular_pixel_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_input("vec4", "v_tint_color")
                    .add_input("vec3", "v_position") 
                    .add_input("vec3", "v_normal")   

                    .add_output("vec4", "frag_color")

                    .add_uniform("float", "u_ambient_strength")
                    .add_uniform("vec3", "u_light_position")   
                    .add_uniform("vec3", "u_light_color")      
                    .add_uniform("vec3", "u_specular_color")   
                    .add_uniform("float", "u_specular_strength")
                    .add_uniform("int", "u_specular_power")     
                    .add_uniform("vec3", "u_view_position")     

                    .set_main_function_body(R"(
                        // ambient
                        vec3 ambient = u_ambient_strength * u_light_color;
                        
                        // diffuse
                        vec3 norm = normalize(v_normal);
                        vec3 light_direction = normalize(u_light_position - v_position);
                        float diff = max(dot(norm, light_direction), 0.0);
                        vec3 diffuse = diff * u_light_color;
                        
                        // specular
                        vec3 view_direction = normalize(u_view_position - v_position);
                        vec3 reflect_dir = reflect(-light_direction, norm);
                        float spec = pow(max(dot(view_direction, reflect_dir), 0.0), u_specular_power);
                        vec3 specular = u_specular_strength * spec * u_specular_color;
                        
                        vec4 result = vec4((ambient + diffuse + specular), 1.0) * v_tint_color;
                        frag_color = result;
                    )").build();
            }

            pool_string instance_specular_vertex_shader_code()
            {
                shader_builder builder;

                return builder
                    .set_version(460)

                    .add_attribute("vec3", "a_position", 0)
                    .add_attribute("vec3", "a_normal", 1)
                    .add_attribute("mat4", "a_inst_mat_model", 2)
                    .add_attribute("vec4", "a_inst_color", 6)

                    .add_uniform("mat4", "u_view_proj")

                    .add_output("vec4", "v_tint_color")
                    .add_output("vec3", "v_position")
                    .add_output("vec3", "v_normal")

                    .set_main_function_body(R"(
                        v_tint_color = a_inst_color;
                        v_position = a_position;  
                        v_normal = a_normal;
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
            }
        }
    }
}