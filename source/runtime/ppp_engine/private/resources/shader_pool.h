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
            namespace unlit
            {
                // batched
                string::string_id color();
                string::string_id texture();
                string::string_id font();
                string::string_id normal();

                // instanced
                string::string_id instance_color();
                string::string_id instance_texture();
                string::string_id instance_normal();
            }

            namespace lit
            {
                // batched
                string::string_id color();
                string::string_id specular();

                // instanced
                string::string_id instance_color();
                string::string_id instance_specular();
            }
        }

        bool initialize();
        void terminate();

        bool has_shader(string::string_id tag);

        u32 add_shader_program(string::string_id tag, std::string_view vs_source, std::string_view fs_source);
        u32 add_shader_program(string::string_id tag, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source);

        u32 get_shader_program(string::string_id tag);
    }
}