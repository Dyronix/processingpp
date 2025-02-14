#pragma once

#include "util/types.h"
#include "memory/memory_types.h"
#include "string/string_id.h"

#include <string>

namespace ppp
{
    namespace shader_pool
    {
        namespace tags
        {
            // batched
            string::string_id unlit_color();
            string::string_id unlit_texture();
            string::string_id unlit_font();
            string::string_id unlit_normal();

            string::string_id lit_specular();

            // instanced
            string::string_id instance_unlit_color();
            string::string_id instance_unlit_texture();
            string::string_id instance_unlit_normal();
                  
            string::string_id instance_lit_specular();
        }

        bool initialize();
        void terminate();

        bool has_shader(string::string_id tag);

        u32 add_shader_program(string::string_id tag, std::string_view vs_source, std::string_view fs_source);
        u32 add_shader_program(string::string_id tag, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source);

        u32 get_shader_program(string::string_id tag);
    }
}