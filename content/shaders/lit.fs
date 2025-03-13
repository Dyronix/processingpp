#version 460 core

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec4 v_color;
in vec4 v_light_position;

out vec4 frag_color;

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int  specular_enabled;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int  specular_enabled;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 u_view_position;
uniform vec3 u_view_target;

uniform DirectionalLight u_directional_light;

uniform int u_num_point_lights;
uniform PointLight u_point_lights[8]; 

uniform int u_shadows_enabled;
uniform sampler2D u_shadow_map;
uniform vec3 u_light_pos;

vec3 calc_dir_light(DirectionalLight light, vec3 normal, vec3 view_direction);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragment_position, vec3 view_direction);
float calc_shadows(vec4 light_space_fragment_position);

void main() 
{   
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
    vec3 result = calc_dir_light(u_directional_light, norm, view_direction);
    // phase 2: point lights
    for(int i = 0; i < u_num_point_lights; i++)
    {
        result += calc_point_light(u_point_lights[i], norm, v_position, view_direction);    
    }
    
    frag_color = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 calc_dir_light(DirectionalLight light, vec3 normal, vec3 view_direction)
{
    vec3 light_dir = normalize(-light.direction);

    // ambient shading
    vec3 ambient = light.ambient * v_color.rgb;
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * v_color.rgb;
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
}

// calculates the color when using a point light.
vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragment_position, vec3 view_direction)
{
    vec3 light_dir = normalize(light.position - fragment_position);

    // ambient shading
    vec3 ambient = light.ambient * v_color.rgb;
    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * v_color.rgb;
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
}

float calc_shadows(vec4 light_space_fragment_position)
{
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
}