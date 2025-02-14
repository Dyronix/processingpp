#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <string>
#include <sstream>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            temp_string unlit_color_vertex_shader_code();
            temp_string unlit_color_pixel_shader_code();
            temp_string instance_unlit_color_vertex_shader_code();
                        
            temp_string unlit_texture_vertex_shader_code();
            temp_string unlit_texture_pixel_shader_code();
            temp_string instance_unlit_texture_vertex_shader_code();
                        
            temp_string unlit_font_vertex_shader_code();
            temp_string unlit_font_pixel_shader_code();
                        
            temp_string unlit_normal_vertex_shader_code();
            temp_string unlit_normal_pixel_shader_code();
            temp_string instance_unlit_normal_vertex_shader_code();
                        
            temp_string specular_vertex_shader_code();
            temp_string specular_pixel_shader_code();
            temp_string instance_specular_vertex_shader_code();
        }
    }
}