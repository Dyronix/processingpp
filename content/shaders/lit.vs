#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec4 a_color;

uniform mat4 u_view_proj;
uniform bool u_wireframe;
uniform vec4 u_wireframe_color;
uniform mat4 u_light_vp;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;
out vec4 v_color;
out vec4 v_light_position;

void main() 
{
    v_position = a_position;  
    v_texcoord = a_texcoord;
    v_normal = a_normal;
    v_color = u_wireframe ? u_wireframe_color : a_color;

    v_light_position = u_light_vp * vec4(a_position, 1.0);
    gl_Position = u_view_proj * vec4(a_position, 1.0);                    
}
