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
            namespace unlit
            {
                std::string color_vertex_shader_code();
                std::string color_pixel_shader_code();
                std::string instance_color_vertex_shader_code();

                std::string texture_vertex_shader_code();
                std::string texture_pixel_shader_code();
                std::string instance_texture_vertex_shader_code();

                std::string font_vertex_shader_code();
                std::string font_pixel_shader_code();

                std::string normal_vertex_shader_code();
                std::string normal_pixel_shader_code();
                std::string instance_normal_vertex_shader_code();

                std::string shadow_depth_vertex_shader_code();
                std::string shadow_depth_fragment_shader_code();
                std::string instance_shadow_depth_vertex_shader_code();

                std::string predepth_vertex_shader_code();
                std::string predepth_fragment_shader_code();
                std::string instance_predepth_vertex_shader_code();
            }

            namespace lit
            {
                std::string color_vertex_shader_code();
                std::string color_pixel_shader_code();
                std::string instance_color_vertex_shader_code();

                std::string texture_vertex_shader_code();
                std::string texture_pixel_shader_code();
                std::string instance_texture_vertex_shader_code();

                std::string specular_vertex_shader_code();
                std::string specular_pixel_shader_code();
                std::string instance_specular_vertex_shader_code();
            }
        }
    }
}