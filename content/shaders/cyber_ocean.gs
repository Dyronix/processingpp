#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

uniform mat4 u_view_proj;

uniform int u_amplitude_x;
uniform int u_amplitude_z;
uniform float u_movement_speed_x;
uniform float u_movement_speed_z;
uniform float u_periodic_scale_x;
uniform float u_periodic_scale_z;
uniform float u_total_time;
uniform float u_cube_size;

in vec4 v_color[];
out vec4 v_frag_color;

void create_vertex(vec3 offset)
{
    float wave_height_x = sin((gl_in[0].gl_Position.x * u_periodic_scale_x) + u_movement_speed_x * u_total_time) * u_amplitude_x;
    float wave_height_z = sin((gl_in[0].gl_Position.z * u_periodic_scale_z) + u_movement_speed_z * u_total_time) * u_amplitude_z;
    float y = wave_height_x + wave_height_z;

    // Interpolating color based on height
    float min_height = -u_amplitude_x - u_amplitude_z;
    float max_height = u_amplitude_x + u_amplitude_z;
    
    float epsilon = 1e-6;
    float range = max_height - min_height;
    float color_factor = (y - min_height) / (range + epsilon);  // Add epsilon to avoid zero division

    vec4 actual_offset = vec4(offset * u_cube_size, 0.0);
    vec4 world_position = gl_in[0].gl_Position + actual_offset + vec4(vec3(0.0, y, 0.0), 1.0f);
    
    gl_Position = u_view_proj * world_position;
    vec3 lerped_color = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), color_factor);
    v_frag_color = vec4(lerped_color, 1.0) * v_color[0];
    
    EmitVertex();
}

void create_cube()
{
    create_vertex(vec3(-1.0, 1.0, 1.0));
    create_vertex(vec3(-1.0, -1.0, 1.0));
    create_vertex(vec3(1.0, 1.0, 1.0));
    create_vertex(vec3(1.0, -1.0, 1.0));

    EndPrimitive();

    create_vertex(vec3(1.0, 1.0, 1.0));
    create_vertex(vec3(1.0, -1.0, 1.0));
    create_vertex(vec3(1.0, 1.0, -1.0));
    create_vertex(vec3(1.0, -1.0, -1.0));

    EndPrimitive();

    create_vertex(vec3(1.0, 1.0, -1.0));
    create_vertex(vec3(1.0, -1.0, -1.0));
    create_vertex(vec3(-1.0, 1.0, -1.0));
    create_vertex(vec3(-1.0, -1.0, -1.0));

    EndPrimitive();

    create_vertex(vec3(-1.0, 1.0, -1.0));
    create_vertex(vec3(-1.0, -1.0, -1.0));
    create_vertex(vec3(-1.0, 1.0, 1.0));
    create_vertex(vec3(-1.0, -1.0, 1.0));

    EndPrimitive();

    create_vertex(vec3(1.0, 1.0, 1.0));
    create_vertex(vec3(1.0, 1.0, -1.0));
    create_vertex(vec3(-1.0, 1.0, 1.0));
    create_vertex(vec3(-1.0, 1.0, -1.0));

    EndPrimitive();

    create_vertex(vec3(-1.0, -1.0, 1.0));
    create_vertex(vec3(-1.0, -1.0, -1.0));
    create_vertex(vec3(1.0, -1.0, 1.0));
    create_vertex(vec3(1.0, -1.0, -1.0));

    EndPrimitive();
}

void main()
{
    create_cube();
}
