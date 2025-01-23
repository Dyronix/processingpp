#include "resources/shader_pool.h"
#include "render/render_shader_compiler.h"
#include "render/render_shader_library.h"
#include "render/render_shader.h"

#include "util/log.h"

#include <glad/glad.h>

#include <unordered_map>

namespace ppp
{
    namespace shader_pool
    {
        namespace internal
        {      
            std::unordered_map<std::string, std::shared_ptr<render::shaders::shader_program>>& get_shader_program_map()
            {
                static std::unordered_map<std::string, std::shared_ptr<render::shaders::shader_program>> s_shader_program_map;

                return s_shader_program_map;
            }
        }

        bool initialize()
        {
            internal::get_shader_program_map().emplace(tags::unlit_color, std::make_shared<render::shaders::shader_program>(render::shaders::unlit_color_vertex_shader_code(), render::shaders::unlit_color_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_unlit_color, std::make_shared<render::shaders::shader_program>(render::shaders::instance_unlit_color_vertex_shader_code(), render::shaders::unlit_color_pixel_shader_code()));
            
            internal::get_shader_program_map().emplace(tags::unlit_texture, std::make_shared<render::shaders::shader_program>(render::shaders::unlit_texture_vertex_shader_code(), render::shaders::unlit_texture_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_unlit_texture, std::make_shared<render::shaders::shader_program>(render::shaders::instance_unlit_texture_vertex_shader_code(), render::shaders::unlit_texture_pixel_shader_code()));

            internal::get_shader_program_map().emplace(tags::unlit_font, std::make_shared<render::shaders::shader_program>(render::shaders::unlit_font_vertex_shader_code(), render::shaders::unlit_font_pixel_shader_code()));

            internal::get_shader_program_map().emplace(tags::unlit_normal, std::make_shared<render::shaders::shader_program>(render::shaders::unlit_normal_vertex_shader_code(), render::shaders::unlit_normal_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_unlit_normal, std::make_shared<render::shaders::shader_program>(render::shaders::instance_unlit_normal_vertex_shader_code(), render::shaders::unlit_normal_pixel_shader_code()));
            
            internal::get_shader_program_map().emplace(tags::lit_specular, std::make_shared<render::shaders::shader_program>(render::shaders::specular_vertex_shader_code(), render::shaders::specular_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_lit_specular, std::make_shared<render::shaders::shader_program>(render::shaders::instance_specular_vertex_shader_code(), render::shaders::specular_pixel_shader_code()));

            return true;
        }

        void terminate()
        {
            internal::get_shader_program_map().clear();
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
                auto shader_program = std::make_shared<render::shaders::shader_program>(vs_source, fs_source);
                auto shader_program_id = shader_program->id();
                internal::get_shader_program_map().emplace(tag, shader_program);
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
                auto shader_program = std::make_shared<render::shaders::shader_program>(vs_source, fs_source, gs_source);
                auto shader_program_id = shader_program->id();
                internal::get_shader_program_map().emplace(tag, shader_program);
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
            if (it != std::cend(internal::get_shader_program_map()))
            {
                return it->second->id();
            }

            log::error("Unable to find shader with tag: {}", tag.data());
            return 0;
        }
    }
}