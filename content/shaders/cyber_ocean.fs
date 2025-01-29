#version 460 core

in vec4 v_frag_color;
out vec4 out_color;

void main() 
{
    out_color = v_frag_color;
}