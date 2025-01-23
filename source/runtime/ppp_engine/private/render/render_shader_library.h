#pragma once

#include "util/types.h"

#include <string>
#include <sstream>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            std::string unlit_color_vertex_shader_code();
            std::string unlit_color_pixel_shader_code();
            std::string instance_unlit_color_vertex_shader_code();
                        
            std::string unlit_texture_vertex_shader_code();
            std::string unlit_texture_pixel_shader_code();
            std::string instance_unlit_texture_vertex_shader_code();
                        
            std::string unlit_font_vertex_shader_code();
            std::string unlit_font_pixel_shader_code();
                        
            std::string unlit_normal_vertex_shader_code();
            std::string unlit_normal_pixel_shader_code();
            std::string instance_unlit_normal_vertex_shader_code();
                        
            std::string specular_vertex_shader_code();
            std::string specular_pixel_shader_code();
            std::string instance_specular_vertex_shader_code();
        }
    }
}