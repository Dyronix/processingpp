#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{
    using shader_program = u32;

    namespace shader_pool
    {
        namespace tags
        {
            static const std::string unlit_color = "unlit_color";
            static const std::string unlit_texture = "unlit_texture";
            static const std::string unlit_font = "unlit_font";
        }

        bool initialize();
        void terminate();

        void add_shader_program(const std::string& tag, shader_program shader);
        void add_shader_program(const std::string& tag, const char* vs_source, const char* fs_source);

        shader_program get_shader_program(std::string_view tag);
    }
}