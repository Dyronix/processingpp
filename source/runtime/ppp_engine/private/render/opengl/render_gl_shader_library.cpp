#include <functional>

#include "render/render_shader_library.h"
#include "render/render_features.h"

#include "string/string_conversions.h"

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            constexpr int MAX_NR_POINT_LIGHTS = 8;

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
                shader_builder& add_struct(const std::string& name, const std::vector<std::pair<std::string, std::string>>& members)
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
                shader_builder& add_ssbo(const std::string& name, const std::string& struct_name, const std::string& variable_name, s32 binding, bool is_array = false)
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
                shader_builder& add_attribute(const std::string& type, const std::string& name, s32 location)
                {
                    m_shader_code << "layout (location = " << location << ") in " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_input(const std::string& type, const std::string& name, bool is_flat = false)
                {
                    if(is_flat) m_shader_code << "flat ";
                    m_shader_code << "in " << type << " " << name << ";\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_output(const std::string& type, const std::string& name, bool is_flat = false)
                {
                    if (is_flat) m_shader_code << "flat ";
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
                shader_builder& add_uniform_array(const std::string& type, const std::string& name, s32 size)
                {
                    m_shader_code << "uniform " << type << " " << name << "[" << size << "]; \n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_function(const std::string& function_code)
                {
                    m_shader_code << function_code << "\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_shader_code(const shader_builder& builder)
                {
                    m_shader_code << builder.build() << "\n";
                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_forward_declared_function(const std::string& return_type, const std::string& function_name, const std::vector<std::pair<std::string, std::string>>& params)
                {
                    m_shader_code << return_type << " " << function_name << "(";
                    for (u64 i = 0; i < params.size(); ++i)
                    {
                        auto& pair = params[i];

                        m_shader_code << pair.first << " " << pair.second;
                        if (i != params.size() - 1)
                        {
                            m_shader_code << ", ";
                        }
                    }
                    m_shader_code << ");";

                    return *this;
                }
                //-------------------------------------------------------------------------
                shader_builder& add_function(const std::string& return_type, const std::string& function_name, const std::vector<std::pair<std::string, std::string>>& params, const std::string& body)
                {
                    m_shader_code << return_type << " " << function_name << "(";
                    for (u64 i = 0; i < params.size(); ++i)
                    {
                        auto& pair = params[i];
                        m_shader_code << pair.first << " " << pair.second;
                        if (i != params.size() - 1)
                        {
                            m_shader_code << ", ";
                        }
                    }
                    m_shader_code << ")\n";
                    m_shader_code << "{\n";
                    m_shader_code << body;
                    m_shader_code << "\n}";

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

            //-------------------------------------------------------------------------
            static shader_builder build_lights(s32 max_nr_point_lights)
            {
                shader_builder lights_builder;

                lights_builder
                    .add_struct(
                        "DirectionalLight",
                        {
                            {"vec3", "direction"},

                            {"vec3", "ambient"},
                            {"vec3", "diffuse"},
                            {"vec3", "specular"},

                            {"int", "specular_enabled"}
                        })

                    .add_struct(
                        "PointLight",
                        {
                            {"vec3", "position"},

                            {"vec3", "ambient"},
                            {"vec3", "diffuse"},
                            {"vec3", "specular"},

                            {"int", "specular_enabled"},

                            {"float", "constant"},
                            {"float", "linear"},
                            {"float", "quadratic"}
                        })

                    .add_uniform("DirectionalLight", "u_directional_light")

                    .add_uniform("int", "u_num_point_lights")
                    .add_uniform_array("PointLight", "u_point_lights", max_nr_point_lights);

                return lights_builder;
            }
            //-------------------------------------------------------------------------
            static shader_builder build_shadow_mapping()
            {
                shader_builder shadow_mapping_builder;

                shadow_mapping_builder
                    .add_uniform("int", "u_shadows_enabled")
                    .add_uniform("sampler2D", "u_shadow_map")
                    .add_uniform("vec3", "u_light_pos");

                return shadow_mapping_builder;
            }

            //-------------------------------------------------------------------------
            static shader_builder build_calc_dir_lights_function()
            {
                shader_builder builder;

                builder.add_function(
                    "vec3", "calc_dir_light",
                    {
                        {"DirectionalLight", "light" },
                        {"vec3", "normal"},
                        {"vec3", "view_direction"},
                        {"vec4", "diffuse_color"}
                    }, R"(
                        vec3 light_dir = normalize(-light.direction);

                        // ambient shading
                        vec3 ambient = light.ambient * diffuse_color.rgb;
                        // diffuse shading
                        float diff = max(dot(normal, light_dir), 0.0);
                        vec3 diffuse = light.diffuse * diff * diffuse_color.rgb;
                        // specular shading
                        vec3 specular = vec3(0.0, 0.0, 0.0);
                        if(light.specular_enabled == 1)
                        {
                            vec3  reflect_dir = reflect(-light_dir, normal);
                            float shininess = 32.0;
                            float spec = pow(max(dot(view_direction, reflect_dir), 0.0), shininess);

                            specular = light.specular * spec;
                        }

                        float shadow = 0.0;
                        if(u_shadows_enabled == 1)
                        {
                            shadow = calc_shadows(v_light_position);
                        }

                        // combine results
                        return (ambient + (1.0 - shadow) * (diffuse + specular));
                    )");

                return builder;
            }

            //-------------------------------------------------------------------------
            static shader_builder build_calc_point_lights_function()
            {
                shader_builder builder;

                builder.add_function(
                    "vec3", "calc_point_light",
                    {
                       {"PointLight", "light" },
                       {"vec3", "normal"},
                       {"vec3", "fragment_position"},
                       {"vec3", "view_direction"},
                       {"vec4", "diffuse_color"}
                    }, R"(
                        vec3 light_dir = normalize(light.position - fragment_position);

                        // ambient shading
                        vec3 ambient = light.ambient * diffuse_color.rgb;
                        // diffuse shading
                        float diff = max(dot(normal, light_dir), 0.0);
                        vec3 diffuse = light.diffuse * diff * diffuse_color.rgb;
                        // specular shading
                        vec3 specular = vec3(0.0, 0.0, 0.0);
                        if(light.specular_enabled == 1)
                        {
                            vec3  reflect_dir = reflect(-light_dir, normal);
                            float shininess = 32.0;
                            float spec = pow(max(dot(view_direction, reflect_dir), 0.0), shininess);

                            specular = light.specular * spec;
                        }
                        // attenuation
                        float dist = length(light.position - fragment_position);
                        float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));      

                        ambient *= attenuation;
                        diffuse *= attenuation;
                        specular *= attenuation;

                        // combine results
                        return (ambient + diffuse + specular);
                        )");

                return builder;
            }

            //-------------------------------------------------------------------------
            static shader_builder build_calc_shadows_function()
            {
                shader_builder builder;

                builder.add_function(
                    "float", "calc_shadows",
                    {
                       { "vec4", "light_space_fragment_position" }
                    }, R"(
                        // perform perspective divide
                        vec3 proj_coords = light_space_fragment_position.xyz / light_space_fragment_position.w;
                        // transform to [0,1] range
                        proj_coords = proj_coords * 0.5 + 0.5;
                        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
                        float closest_depth = texture(u_shadow_map, proj_coords.xy).r; 
                        // get depth of current fragment from light's perspective
                        float current_depth = proj_coords.z;

                        // calculate bias (based on depth map resolution and slope)
                        vec3 normal = normalize(v_normal);
                        vec3 light_dir = normalize(u_light_pos - v_position);
                        float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);

                        // check whether current frag pos is in shadow
                        // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
                        // PCF
                        float shadow = 0.0;
                        vec2 texel_size = 1.0 / textureSize(u_shadow_map, 0);
                        for(int x = -1; x <= 1; ++x)
                        {
                            for(int y = -1; y <= 1; ++y)
                            {
                                float pcf_depth = texture(u_shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r; 
                                shadow += current_depth - bias > pcf_depth  ? 1.0 : 0.0;        
                            }    
                        }
                        shadow /= 9.0;
                        
                        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
                        if(proj_coords.z > 1.0)
                        {
                            shadow = 0.0;
                        }

                        return shadow;
                        )");

                return builder;
            }

            namespace unlit
            {
                //-------------------------------------------------------------------------
                std::string color_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string color_pixel_shader_code()
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

                //-------------------------------------------------------------------------
                std::string instance_color_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string texture_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string texture_pixel_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_struct(
                            "material_attributes",
                            {
                                {"int", "sampler_indices[" + string::to_string<std::string>(render::max_textures()) + "]"},
                                {"int", "sampler_count"},
                                {"vec4", "ambient_color"},
                                {"vec4", "diffuse_color"}
                            })
                        .add_ssbo("material_buffer", "material_attributes", "materials", 1, true)

                        .add_uniform_array("sampler2D", "u_image_samplers", render::max_textures())

                        .add_input("vec4", "v_tint_color")
                        .add_input("vec2", "v_texture")
                        .add_input("int", "v_material_idx", true)

                        .add_output("vec4", "frag_color")

                        .set_main_function_body(R"(
                        vec4 base_color = texture(u_image_samplers[materials[v_material_idx].sampler_indices[0]], v_texture);
                        frag_color = base_color;
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string instance_texture_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string font_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string font_pixel_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_uniform_array("sampler2D", "u_image_samplers", render::max_textures())

                        .add_input("vec4", "v_tint_color")
                        .add_input("vec2", "v_texture")
                        .add_input("int", "v_texture_idx", true)

                        .add_output("vec4", "frag_color")

                        .set_main_function_body(R"(
                        vec4 color = vec4(v_tint_color.rgb, texture(u_image_samplers[v_texture_idx], v_texture).r); 
                        frag_color = color;
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string normal_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string normal_pixel_shader_code()
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

                //-------------------------------------------------------------------------
                std::string instance_normal_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string shadow_depth_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)
                        .add_attribute("vec3", "a_position", 0)                       
                        .add_uniform("mat4", "u_view_proj")

                        .set_main_function_body(R"(
                        gl_Position = u_view_proj * vec4(a_position, 1.0);
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string shadow_depth_fragment_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)
                        .set_main_function_body(R"(
                        // gl_FragDepth = gl_FragCoord.z;
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string instance_shadow_depth_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)
                        .add_attribute("vec3", "a_position", 0)
                        .add_attribute("mat4", "a_inst_mat_model", 1)
                        .add_attribute("vec4", "a_inst_color", 5)
                        .add_uniform("mat4", "u_view_proj")
                        .set_main_function_body(R"(
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string predepth_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)
                        .add_attribute("vec3", "a_position", 0)
                        .add_uniform("mat4", "u_view_proj")

                        .set_main_function_body(R"(
                        gl_Position = u_view_proj * vec4(a_position, 1.0);
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string predepth_fragment_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)
                        .set_main_function_body(R"(
                        // gl_FragDepth = gl_FragCoord.z;
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string instance_predepth_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)
                        .add_attribute("vec3", "a_position", 0)
                        .add_attribute("mat4", "a_inst_mat_model", 1)
                        .add_attribute("vec4", "a_inst_color", 5)
                        .add_uniform("mat4", "u_view_proj")
                        .set_main_function_body(R"(
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
                }
            }

            namespace lit
            {
                //-------------------------------------------------------------------------
                std::string color_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_attribute("vec3", "a_position", 0)
                        .add_attribute("vec3", "a_normal", 1)
                        .add_attribute("vec4", "a_color", 2)

                        .add_uniform("mat4", "u_view_proj")
                        .add_uniform("bool", "u_wireframe")
                        .add_uniform("vec4", "u_wireframe_color")
                        .add_uniform("mat4", "u_light_vp")

                        .add_output("vec3", "v_position")
                        .add_output("vec3", "v_normal")
                        .add_output("vec4", "v_color")
                        .add_output("vec4", "v_light_position")

                        .set_main_function_body(R"(
                        v_position = a_position;  
                        v_normal = a_normal;
                        v_color = u_wireframe ? u_wireframe_color : a_color;
                        v_light_position = u_light_vp * vec4(a_position, 1.0);
                        gl_Position = u_view_proj * vec4(a_position, 1.0);
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string color_pixel_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_input("vec3", "v_position")
                        .add_input("vec3", "v_normal")
                        .add_input("vec4", "v_color")
                        .add_input("vec4", "v_light_position")

                        .add_output("vec4", "frag_color")

                        .add_shader_code(build_lights(MAX_NR_POINT_LIGHTS))
                        .add_shader_code(build_shadow_mapping())

                        .add_uniform("vec3", "u_view_position")
                        .add_uniform("vec3", "u_view_target")

                        .add_shader_code(build_calc_shadows_function())
                        .add_shader_code(build_calc_dir_lights_function())
                        .add_shader_code(build_calc_point_lights_function())
                        
                        .set_main_function_body(R"(
                            // Common calculations
                            vec3 norm = normalize(v_normal);
                            vec3 view_direction = normalize(u_view_position - v_position);

                            // == =====================================================
                            // Our lighting is set up in 2 phases: directional and point lights
                            // For each phase, a calculate function is defined that calculates the corresponding color
                            // per lamp. In the main() function we take all the calculated colors and sum them up for
                            // this fragment's final color.
                            // == =====================================================
                            // phase 1: directional lighting
                            vec3 result = calc_dir_light(u_directional_light, norm, view_direction, v_color);
                            // phase 2: point lights
                            for(int i = 0; i < u_num_point_lights; i++)
                            {
                                result += calc_point_light(u_point_lights[i], norm, v_position, view_direction, v_color);    
                            }
                            
                            frag_color = vec4(result, 1.0);
                        )").build();
                }

                //-------------------------------------------------------------------------
                std::string instance_color_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_attribute("vec3", "a_position", 0)
                        .add_attribute("vec3", "a_normal", 1)
                        .add_attribute("vec4", "a_color", 2)
                        .add_attribute("mat4", "a_inst_mat_model", 3)
                        .add_attribute("vec4", "a_inst_color", 7)

                        .add_uniform("mat4", "u_view_proj")
                        .add_uniform("bool", "u_wireframe")
                        .add_uniform("vec4", "u_wireframe_color")
                        .add_uniform("mat4", "u_light_vp")

                        .add_output("vec3", "v_position")
                        .add_output("vec3", "v_normal")
                        .add_output("vec4", "v_color")
                        .add_output("vec4", "v_light_position")

                        .set_main_function_body(R"(
                        v_position = a_position;  
                        v_normal = a_normal;
                        v_color = u_wireframe ? u_wireframe_color : a_color;
                        v_light_position = u_light_vp * a_inst_mat_model * vec4(a_position, 1.0);
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string texture_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_attribute("vec3", "a_position", 0)
                        .add_attribute("vec3", "a_normal", 1)
                        .add_attribute("vec2", "a_texture", 2)
                        .add_attribute("vec4", "a_tint_color", 3)
                        .add_attribute("int", "a_material_idx", 4)

                        .add_uniform("mat4", "u_view_proj")
                        .add_uniform("bool", "u_wireframe")
                        .add_uniform("vec4", "u_wireframe_color")
                        .add_uniform("mat4", "u_light_vp")

                        .add_output("vec3", "v_position")
                        .add_output("vec3", "v_normal")
                        .add_output("vec4", "v_tint_color")
                        .add_output("vec2", "v_texture")
                        .add_output("int", "v_material_idx", true)
                        .add_output("vec4", "v_light_position")

                        .set_main_function_body(R"(
                        v_position = a_position;  
                        v_normal = a_normal;
                        v_tint_color = u_wireframe ? u_wireframe_color : a_tint_color;
                        v_texture = a_texture;                                        
                        v_material_idx = a_material_idx;
                        v_light_position = u_light_vp * vec4(a_position, 1.0);                              
                        gl_Position = u_view_proj * vec4(a_position, 1.0);            
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string texture_pixel_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_struct(
                            "material_attributes",
                            {
                                {"int", "sampler_indices[" + string::to_string<std::string>(render::max_textures()) + "]"},
                                {"int", "sampler_count"},
                                {"vec4", "ambient_color"},
                                {"vec4", "diffuse_color"}
                            })
                        .add_ssbo("material_buffer", "material_attributes", "materials", 1, true)

                        .add_uniform_array("sampler2D", "u_image_samplers", render::max_textures())

                        .add_input("vec3", "v_position")
                        .add_input("vec3", "v_normal")
                        .add_input("vec4", "v_tint_color")
                        .add_input("vec2", "v_texture")
                        .add_input("int", "v_material_idx", true)
                        .add_input("vec4", "v_light_position")

                        .add_output("vec4", "frag_color")

                        .add_shader_code(build_lights(MAX_NR_POINT_LIGHTS))
                        .add_shader_code(build_shadow_mapping())

                        .add_uniform("vec3", "u_view_position")
                        .add_uniform("vec3", "u_view_target")

                        .add_shader_code(build_calc_shadows_function())
                        .add_shader_code(build_calc_dir_lights_function())
                        .add_shader_code(build_calc_point_lights_function())

                        .set_main_function_body(R"(
                            vec4 tex_color = texture(u_image_samplers[materials[v_material_idx].sampler_indices[0]], v_texture);
                            vec4 diff_color = tex_color * v_tint_color;                            

                            // Common calculations
                            vec3 norm = normalize(v_normal);
                            vec3 view_direction = normalize(u_view_position - v_position);

                            // == =====================================================
                            // Our lighting is set up in 2 phases: directional and point lights
                            // For each phase, a calculate function is defined that calculates the corresponding color
                            // per lamp. In the main() function we take all the calculated colors and sum them up for
                            // this fragment's final color.
                            // == =====================================================
                            // phase 1: directional lighting
                            vec3 result = calc_dir_light(u_directional_light, norm, view_direction, diff_color);
                            // phase 2: point lights
                            for(int i = 0; i < u_num_point_lights; i++)
                            {
                                result += calc_point_light(u_point_lights[i], norm, v_position, view_direction, diff_color);    
                            }
                            
                            frag_color = vec4(result, 1.0);
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string instance_texture_vertex_shader_code()
                {
                    shader_builder builder;

                    return builder
                        .set_version(460)

                        .add_attribute("vec3", "a_position", 0)
                        .add_attribute("vec3", "a_normal", 1)
                        .add_attribute("vec2", "a_texture", 2)
                        .add_attribute("int", "a_inst_material_idx", 3)
                        .add_attribute("mat4", "a_inst_mat_model", 4)
                        .add_attribute("vec4", "a_inst_color", 8)

                        .add_uniform("mat4", "u_view_proj")
                        .add_uniform("bool", "u_wireframe")
                        .add_uniform("vec4", "u_wireframe_color")
                        .add_uniform("mat4", "u_light_vp")

                        .add_output("vec3", "v_position")
                        .add_output("vec3", "v_normal")
                        .add_output("vec4", "v_tint_color")
                        .add_output("vec2", "v_texture")
                        .add_output("int", "v_material_idx", true)
                        .add_output("vec4", "v_light_position")

                        .set_main_function_body(R"(
                        v_position = a_position;  
                        v_normal = a_normal;
                        v_tint_color = u_wireframe ? u_wireframe_color : a_inst_color;
                        v_texture = a_texture;
                        v_material_idx = a_inst_material_idx;
                        v_light_position = u_light_vp * vec4(a_position, 1.0);   
                        gl_Position = u_view_proj * a_inst_mat_model * vec4(a_position, 1.0);
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string specular_vertex_shader_code()
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

                //-------------------------------------------------------------------------
                std::string specular_pixel_shader_code()
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
                        .add_uniform("int", "u_blinn")

                        .set_main_function_body(R"(
                        // ambient
                        vec3 ambient = u_ambient_strength * u_light_color;
                        
                        // diffuse
                        vec3 norm = normalize(v_normal);
                        vec3 light_direction = normalize(u_light_position - v_position);
                        float diff = max(dot(light_direction, norm), 0.0);
                        vec3 diffuse = v_tint_color.rgb * diff * u_light_color;
                        
                        // specular
                        vec3 view_direction = normalize(u_view_position - v_position);
                        float spec = 0.0;           
                        
                        if(u_blinn == 1)
                        {
                            vec3 halfway_dir = normalize(light_direction + view_direction);  
                            spec = pow(max(dot(norm, halfway_dir), 0.0), u_specular_power);
                        }
                        else
                        {
                            vec3 reflect_dir = reflect(-light_direction, norm);
                            spec = pow(max(dot(view_direction, reflect_dir), 0.0), u_specular_power);
                        }
                        
                        vec3 specular = u_specular_strength * spec * u_specular_color;
                        vec4 result = vec4((ambient + diffuse + specular), 1.0);
                        frag_color = result;
                    )").build();
                }

                //-------------------------------------------------------------------------
                std::string instance_specular_vertex_shader_code()
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
}