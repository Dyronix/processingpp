#include "resources/shader_pool.h"
#include "render/render_shader_compiler.h"
#include "render/render_shader_library.h"
#include "render/render_shader.h"

#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"

#include "util/log.h"

#include <glad/glad.h>

#include <unordered_map>

namespace ppp
{
    namespace shader_pool
    {
        namespace internal
        {      
            graphics_hash_map<string::string_id, std::shared_ptr<render::shaders::shader_program>>& get_shader_program_map()
            {
                static auto s_shader_program_map = memory::tagged_placement_new<graphics_hash_map<string::string_id, std::shared_ptr<render::shaders::shader_program>>>();

                return *s_shader_program_map;
            }
        }

        namespace tags
        {
            // batched
            string::string_id unlit_color()
            {
                static const string::string_id s_unlit_color = string::store_sid("unlit_color");

                return s_unlit_color;
            }
            string::string_id unlit_texture()
            {
                static const string::string_id s_unlit_texture = string::store_sid("unlit_texture");

                return s_unlit_texture;
            }
            string::string_id unlit_font()
            {
                static const string::string_id s_unlit_font = string::store_sid("unlit_font");

                return s_unlit_font;
            }
            string::string_id unlit_normal()
            {
                static const string::string_id s_unlit_normal = string::store_sid("unlit_normal");

                return s_unlit_normal;
            }

            string::string_id lit_specular()
            {
                static const string::string_id s_lit_specular = string::store_sid("lit_specular");

                return s_lit_specular;
            }

            // instanced
            string::string_id instance_unlit_color()
            {
                static const string::string_id s_instance_unlit_color = string::store_sid("instance_unlit_color");

                return s_instance_unlit_color;
            }
            string::string_id instance_unlit_texture()
            {
                static const string::string_id s_instance_unlit_texture = string::store_sid("instance_unlit_texture");

                return s_instance_unlit_texture;
            }
            string::string_id instance_unlit_normal()
            {
                static const string::string_id s_instance_unlit_normal = string::store_sid("instance_unlit_normal");

                return s_instance_unlit_normal;
            }

            string::string_id instance_lit_specular()
            {
                static const string::string_id s_instance_lit_specular = string::store_sid("instance_lit_specular");

                return s_instance_lit_specular;
            }
        }

        bool initialize()
        {
            memory::persistent_graphics_tagged_allocator<render::shaders::shader_program> graphics_allocator;

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

        bool has_shader(string::string_id tag)
        {
            auto it = internal::get_shader_program_map().find(tag);
            return it != std::cend(internal::get_shader_program_map());
        }

        u32 add_shader_program(string::string_id tag, std::string_view vs_source, std::string_view fs_source)
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
                log::warn("shader tag {} already exists", string::restore_sid(tag));
            }

            return -1;
        }

        u32 add_shader_program(string::string_id tag, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source)
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
                log::warn("shader tag {} already exists", string::restore_sid(tag));
            }

            return -1;
        }

        u32 get_shader_program(string::string_id tag)
        {
            auto it = internal::get_shader_program_map().find(tag);
            if (it != std::cend(internal::get_shader_program_map()))
            {
                return it->second->id();
            }

            log::error("Unable to find shader with tag: {}", string::restore_sid(tag));
            return 0;
        }
    }
}