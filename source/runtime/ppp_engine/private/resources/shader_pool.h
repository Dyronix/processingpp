#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace shader_pool
    {
        namespace tags
        {
            // batched
            static const std::string unlit_color = "unlit_color";
            static const std::string unlit_texture = "unlit_texture";
            static const std::string unlit_font = "unlit_font";
            static const std::string unlit_normal = "unlit_normal";

            static const std::string lit_specular = "lit_specular";

            // instanced
            static const std::string instance_unlit_color = "instance_unlit_color";
            static const std::string instance_unlit_texture = "instance_unlit_texture";
            static const std::string instance_unlit_normal = "instance_unlit_normal";

            static const std::string instance_lit_specular = "instance_lit_specular";
        }

        bool initialize();
        void terminate();

        bool has_shader(const std::string& tag);

        u32 add_shader_program(const std::string& tag, const char* vs_source, const char* fs_source);
        u32 add_shader_program(const std::string& tag, const char* vs_source, const char* fs_source, const char* gs_source);

        u32 get_shader_program(std::string_view tag);
    }
}