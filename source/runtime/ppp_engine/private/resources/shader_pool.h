#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace shader_pool
    {
        namespace tags
        {
            static const std::string unlit_color = "unlit_color";
            static const std::string unlit_texture = "unlit_texture";
            static const std::string unlit_font = "unlit_font";
            static const std::string unlit_normal = "unlit_normal";
            static const std::string unlit_specular = "unlit_specular";
        }

        bool initialize();
        void terminate();

        bool has_shader(const std::string& tag);

        u32 add_shader_program(const std::string& tag, const char* vs_source, const char* fs_source);
        u32 add_shader_program(const std::string& tag, const char* vs_source, const char* fs_source, const char* gs_source);

        u32 get_shader_program(std::string_view tag);
    }
}