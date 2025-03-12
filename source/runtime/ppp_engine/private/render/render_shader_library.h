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
            namespace unlit
            {
                temp_string color_vertex_shader_code();
                temp_string color_pixel_shader_code();
                temp_string instance_color_vertex_shader_code();

                temp_string texture_vertex_shader_code();
                temp_string texture_pixel_shader_code();
                temp_string instance_texture_vertex_shader_code();

                temp_string font_vertex_shader_code();
                temp_string font_pixel_shader_code();

                temp_string normal_vertex_shader_code();
                temp_string normal_pixel_shader_code();
                temp_string instance_normal_vertex_shader_code();

                temp_string shadow_depth_vertex_shader_code();
                temp_string shadow_depth_fragment_shader_code();
                temp_string instance_shadow_depth_vertex_shader_code();
            }

            namespace lit
            {
                temp_string color_vertex_shader_code();
                temp_string color_pixel_shader_code();
                temp_string instance_color_vertex_shader_code();

                temp_string specular_vertex_shader_code();
                temp_string specular_pixel_shader_code();
                temp_string instance_specular_vertex_shader_code();
            }
        }
    }
}