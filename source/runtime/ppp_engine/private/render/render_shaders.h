#pragma once

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            const char* const unlit_color_vertex_shader_code();
            const char* const unlit_color_pixel_shader_code();

            const char* const unlit_texture_vertex_shader_code();
            const char* const unlit_texture_pixel_shader_code();

            const char* const unlit_font_vertex_shader_code();
            const char* const unlit_font_pixel_shader_code();
        
            u32 create_shader_program(const char* vs_source, const char* fs_soruce);
        }
    }
}