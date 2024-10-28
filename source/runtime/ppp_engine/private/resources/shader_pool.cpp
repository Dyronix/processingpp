#include "resources/shader_pool.h"
#include "render/render_shaders.h"

#include "util/log.h"

#include <glad/glad.h>

#include <unordered_map>

namespace ppp
{
    namespace shader_pool
    {
        namespace internal
        {      
            std::unordered_map<std::string, u32>& get_shader_program_map()
            {
                static std::unordered_map<std::string, u32> s_shader_program_map;

                return s_shader_program_map;
            }
        }

        bool initialize()
        {
            internal::get_shader_program_map()[tags::unlit_color] = render::shaders::create_shader_program(render::shaders::unlit_color_vertex_shader_code(), render::shaders::unlit_color_pixel_shader_code());
            internal::get_shader_program_map()[tags::unlit_texture] = render::shaders::create_shader_program(render::shaders::unlit_texture_vertex_shader_code(), render::shaders::unlit_texture_pixel_shader_code());
            internal::get_shader_program_map()[tags::unlit_font] = render::shaders::create_shader_program(render::shaders::unlit_font_vertex_shader_code(), render::shaders::unlit_font_pixel_shader_code());
            internal::get_shader_program_map()[tags::unlit_normal] = render::shaders::create_shader_program(render::shaders::unlit_normal_vertex_shader_code(), render::shaders::unlit_normal_pixel_shader_code());

            return true;
        }

        void terminate()
        {
            for(auto pair : internal::get_shader_program_map())
            {
                glDeleteProgram(pair.second);
            }
        }

        bool has_shader(const std::string& tag)
        {
            auto it = internal::get_shader_program_map().find(tag);
            return it != std::cend(internal::get_shader_program_map());
        }

        u32 add_shader_program(const std::string& tag, const char* vs_source, const char* fs_source)
        {
            auto it = internal::get_shader_program_map().find(tag);
            if(it == std::cend(internal::get_shader_program_map()))
            {
                u32 shader_program_id = render::shaders::create_shader_program(vs_source, fs_source);
                internal::get_shader_program_map().emplace(tag, shader_program_id);
                return shader_program_id;
            }
            else
            {
                log::warn("shader tag {} already exists", tag);
            }

            return -1;
        }

        u32 add_shader_program(const std::string& tag, const char* vs_source, const char* fs_source, const char* gs_source)
        {
            auto it = internal::get_shader_program_map().find(tag);
            if (it == std::cend(internal::get_shader_program_map()))
            {
                u32 shader_program_id = render::shaders::create_shader_program(vs_source, fs_source, gs_source);
                internal::get_shader_program_map().emplace(tag, shader_program_id);
                return shader_program_id;
            }
            else
            {
                log::warn("shader tag {} already exists", tag);
            }

            return -1;
        }

        u32 get_shader_program(std::string_view tag)
        {
            auto it = internal::get_shader_program_map().find(tag.data());
            return it != std::cend(internal::get_shader_program_map())
                ? it->second
                : 0;
        }
    }
}