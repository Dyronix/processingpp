#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <string>

namespace ppp
{
    namespace shader_pool
    {
        namespace tags
        {
            // batched
            const pool_string& unlit_color();
            const pool_string& unlit_texture();
            const pool_string& unlit_font();
            const pool_string& unlit_normal();

            const pool_string& lit_specular();

            // instanced
            const pool_string& instance_unlit_color();
            const pool_string& instance_unlit_texture();
            const pool_string& instance_unlit_normal();
                  
            const pool_string& instance_lit_specular();
        }

        bool initialize();
        void terminate();

        bool has_shader(std::string_view tag);

        u32 add_shader_program(std::string_view tag, std::string_view vs_source, std::string_view fs_source);
        u32 add_shader_program(std::string_view tag, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source);

        u32 get_shader_program(std::string_view tag);
    }
}