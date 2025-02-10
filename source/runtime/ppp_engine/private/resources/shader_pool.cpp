#include "resources/shader_pool.h"
#include "render/render_shader_compiler.h"
#include "render/render_shader_library.h"
#include "render/render_shader.h"

#include "memory/memory_types.h"

#include "util/log.h"

#include <glad/glad.h>

#include <unordered_map>

namespace ppp
{
    namespace shader_pool
    {
        namespace internal
        {      
            graphics_hash_map<std::string_view, std::shared_ptr<render::shaders::shader_program>>& get_shader_program_map()
            {
                static graphics_hash_map<std::string_view, std::shared_ptr<render::shaders::shader_program>> s_shader_program_map;

                return s_shader_program_map;
            }
        }

        namespace tags
        {
            // batched
            const pool_string& unlit_color()
            {
                static const pool_string s_unlit_color = "unlit_color";

                return s_unlit_color;
            }
            const pool_string& unlit_texture()
            {
                static const pool_string s_unlit_texture = "unlit_texture";

                return s_unlit_texture;
            }
            const pool_string& unlit_font()
            {
                static const pool_string s_unlit_font = "unlit_font";

                return s_unlit_font;
            }
            const pool_string& unlit_normal()
            {
                static const pool_string s_unlit_normal = "unlit_normal";

                return s_unlit_normal;
            }

            const pool_string& lit_specular()
            {
                static const pool_string s_lit_specular = "lit_specular";

                return s_lit_specular;
            }

            // instanced
            const pool_string& instance_unlit_color()
            {
                static const pool_string s_instance_unlit_color = "instance_unlit_color";

                return s_instance_unlit_color;
            }
            const pool_string& instance_unlit_texture()
            {
                static const pool_string s_instance_unlit_texture = "instance_unlit_texture";

                return s_instance_unlit_texture;
            }
            const pool_string& instance_unlit_normal()
            {
                static const pool_string s_instance_unlit_normal = "instance_unlit_normal";

                return s_instance_unlit_normal;
            }

            const pool_string& instance_lit_specular()
            {
                static const pool_string s_instance_lit_specular = "instance_lit_specular";

                return s_instance_lit_specular;
            }
        }

        bool initialize()
        {
            memory::tagged_allocator<render::shaders::shader_program, memory::tags::graphics> graphics_allocator;

            internal::get_shader_program_map().emplace(tags::unlit_color(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::unlit_color_vertex_shader_code(), render::shaders::unlit_color_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_unlit_color(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::instance_unlit_color_vertex_shader_code(), render::shaders::unlit_color_pixel_shader_code()));
            
            internal::get_shader_program_map().emplace(tags::unlit_texture(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::unlit_texture_vertex_shader_code(), render::shaders::unlit_texture_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_unlit_texture(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::instance_unlit_texture_vertex_shader_code(), render::shaders::unlit_texture_pixel_shader_code()));

            internal::get_shader_program_map().emplace(tags::unlit_font(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::unlit_font_vertex_shader_code(), render::shaders::unlit_font_pixel_shader_code()));

            internal::get_shader_program_map().emplace(tags::unlit_normal(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::unlit_normal_vertex_shader_code(), render::shaders::unlit_normal_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_unlit_normal(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::instance_unlit_normal_vertex_shader_code(), render::shaders::unlit_normal_pixel_shader_code()));
            
            internal::get_shader_program_map().emplace(tags::lit_specular(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::specular_vertex_shader_code(), render::shaders::specular_pixel_shader_code()));
            internal::get_shader_program_map().emplace(tags::instance_lit_specular(), std::allocate_shared<render::shaders::shader_program>(graphics_allocator, render::shaders::instance_specular_vertex_shader_code(), render::shaders::specular_pixel_shader_code()));

            return true;
        }

        void terminate()
        {
            internal::get_shader_program_map().clear();
        }

        bool has_shader(std::string_view tag)
        {
            auto it = internal::get_shader_program_map().find(tag);
            return it != std::cend(internal::get_shader_program_map());
        }

        u32 add_shader_program(std::string_view tag, std::string_view vs_source, std::string_view fs_source)
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

        u32 add_shader_program(std::string_view tag, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source)
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
            auto it = internal::get_shader_program_map().find(tag);
            if (it != std::cend(internal::get_shader_program_map()))
            {
                return it->second->id();
            }

            log::error("Unable to find shader with tag: {}", tag);
            return 0;
        }
    }
}