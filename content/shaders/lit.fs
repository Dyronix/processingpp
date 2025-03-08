#version 460 core

in vec3 v_position;
in vec3 v_normal;
in vec4 v_color;

out vec4 frag_color;

uniform vec3 u_view_position;
uniform vec3 u_view_target;

struct AmbientLight
{
    vec3 color;
    float intensity;
};

uniform AmbientLight u_ambient_light;
struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
    vec3 specular_color;
    int specular_enabled;
};

uniform DirectionalLight u_directional_light;
struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    vec3 specular_color;
    int specular_enabled;
    float range;        // the maximum effective range of the light
    float falloff;      // a value controlling the decay (try values in a sensible range)
    float threshold;    // the desired attenuation at the range (e.g., 0.01)
};

uniform int u_num_point_lights;
uniform PointLight u_point_lights[8]; 

vec3 compute_attenuation_coefficients(float range, float falloff, float threshold) 
{
    // Constant term is fixed at 1.0.
    float Kc = 1.0;
    // Map falloff to the linear term.
    float Kl = falloff / range;
    // Determine quadratic term so that at d = range, attenuation is 1/threshold.
    float Kq = (1.0/threshold - Kc - falloff) / (range * range);

    return vec3(Kc, Kl, Kq);
}

float compute_attenuation(float d, vec3 coeffs) 
{
    // Compute attenuation factor given distance d and coefficients.
    float denom = coeffs.x + coeffs.y * d + coeffs.z * d * d;

    return 1.0 / denom;
}


void main() 
{   
    // Common calculations
    vec3 norm = normalize(v_normal);
    vec3 view_direction = normalize(u_view_position - v_position);

    // Ambient
    // Ambient Light | Ambient contribution
    //
    float ambient_strength = u_ambient_light.intensity;
    vec3 ambient_light_result = (ambient_strength * u_ambient_light.color) * v_color.rgb;

    // Directional
    // Directional Light | Diffuse contribution.
    //
    vec3 dir_light_dir = normalize(-u_directional_light.direction);
    float dir_diff_amount = max(dot(norm, dir_light_dir), 0.0) * u_directional_light.intensity;
    vec3 dir_diffuse = (dir_diff_amount * u_directional_light.color) * v_color.rgb;

    // Directional Light | Specular contribution.
    //
    float shininess = 32.0;
    vec3 dir_specular = vec3(0.0);
    if(u_directional_light.specular_enabled == 1)
    {
        vec3 dir_reflection_dir = reflect(-u_directional_light.direction, norm);
        float dir_spec_amount = pow(max(dot(view_direction, dir_reflection_dir), 0.0), shininess);
        dir_specular = dir_spec_amount * u_directional_light.specular_color;
    }

    // Directional Light | Sum contributions from this directional light.
    //
    vec3 directional_light_result = dir_diffuse + dir_specular;

    // Point
    vec3 point_light_result = vec3(0.0);
    for (int i = 0; i < u_num_point_lights; i++)
    {
        // Point Light | Diffuse contribution.
        //
        vec3 point_light_dir = normalize(u_point_lights[i].position - v_position);
        float point_diff_amount = max(dot(norm, point_light_dir), 0.0) * u_point_lights[i].intensity;
        vec3 point_diffuse = (point_diff_amount * u_point_lights[i].color) * v_color.rgb;
        
        // Point Light | Specular contribution.
        //
        float shininess = 32.0;
        vec3 point_specular = vec3(0.0);
        if(u_point_lights[i].specular_enabled == 1)
        {
            vec3 point_reflection_dir = reflect(-point_light_dir, norm);
            float point_spec_amount = pow(max(dot(view_direction, point_reflection_dir), 0.0), shininess);
            point_specular = u_point_lights[i].specular_color * point_spec_amount * v_color.rgb;
        }
        
        // Point Light | Attenuation contribution.
        //
        float dist          = length(u_point_lights[i].position - v_position);
        vec3  coeff         = compute_attenuation_coefficients(u_point_lights[i].range, u_point_lights[i].falloff, u_point_lights[i].threshold);
        float attenuation   = compute_attenuation(dist, coeff);
                                   
        point_diffuse       *= attenuation;
        point_specular      *= attenuation;
        
        // Point Light | Sum contributions from this point light.
        //
        point_light_result += point_diffuse + point_specular;
    }

    // --- Final Color ---
    vec3 result = ambient_light_result + directional_light_result + point_light_result;
    frag_color = vec4(result, 1.0);
}
