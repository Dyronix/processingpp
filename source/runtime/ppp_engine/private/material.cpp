#include "material.h"
#include "fileio/fileio.h"
#include "render/render_shader_uniform_manager.h"
#include "render/render.h"
#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "util/log.h"
#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"
#include "string/string_id.h"

namespace ppp
{
    namespace material
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static string::string_id& active_shader_tag()
            {
                static string::string_id s_active_shader_tag = {};

                return s_active_shader_tag;
            }

            //-------------------------------------------------------------------------
            static graphics_hash_map<string::string_id, render::vertex_type>& shader_tag_vertex_type_map()
            {
                static auto s_shader_tag_vertex_types = std::initializer_list<std::pair<const string::string_id, render::vertex_type>>
                {
                    {shader_pool::tags::unlit_color(),              render::vertex_type::POSITION_COLOR},
                    {shader_pool::tags::instance_unlit_color(),     render::vertex_type::POSITION},

                    {shader_pool::tags::unlit_texture(),            render::vertex_type::POSITION_TEXCOORD_COLOR},
                    {shader_pool::tags::instance_unlit_texture(),   render::vertex_type::POSITION_TEXCOORD},

                    {shader_pool::tags::unlit_font(),               render::vertex_type::POSITION_TEXCOORD_COLOR},

                    {shader_pool::tags::unlit_normal(),             render::vertex_type::POSITION_NORMAL_COLOR},
                    {shader_pool::tags::instance_unlit_normal(),    render::vertex_type::POSITION_NORMAL},

                    {shader_pool::tags::lit_specular(),             render::vertex_type::POSITION_NORMAL_COLOR},
                    {shader_pool::tags::instance_lit_specular(),    render::vertex_type::POSITION_NORMAL}
                };

                static auto s_shader_tag_vertex_type_map = memory::tagged_placement_new<graphics_hash_map<string::string_id, render::vertex_type>>(s_shader_tag_vertex_types);

                return *s_shader_tag_vertex_type_map;
            }
        }

        namespace tags
        {
            //-------------------------------------------------------------------------
            // batched
            std::string_view unlit_color()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit_color()
                    : shader_pool::tags::instance_unlit_color();

                return string::restore_sid(sid);
            }
            //-------------------------------------------------------------------------
            std::string_view unlit_texture()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit_texture()
                    : shader_pool::tags::instance_unlit_texture();

                return string::restore_sid(sid);
            }
            //-------------------------------------------------------------------------
            std::string_view unlit_font()
            {
                auto sid = shader_pool::tags::unlit_font();

                return string::restore_sid(sid);
            }
            //-------------------------------------------------------------------------
            std::string_view unlit_normal()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit_normal()
                    : shader_pool::tags::instance_unlit_normal();

                return string::restore_sid(sid);
            }

            //-------------------------------------------------------------------------
            std::string_view lit_specular()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::lit_specular()
                    : shader_pool::tags::instance_lit_specular();

                return string::restore_sid(sid);
            }
        }

        //-------------------------------------------------------------------------
        shader_program::shader_program()
            :id(-1)
        {}

        //-------------------------------------------------------------------------
        shader_program::shader_program(shader_program_id in_id)
            :id(in_id)
        {}

        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, bool value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, int value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, float value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, const glm::vec2& value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, const glm::vec3& value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, const glm::vec4& value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, const glm::mat2& value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, const glm::mat3& value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(std::string_view uniform_name, const glm::mat4& value)
        {
            render::shaders::push_uniform(id, string::store_sid(uniform_name), value);
        }

        //-------------------------------------------------------------------------
        void texture(unsigned int image_id)
        {
            assert(!internal::active_shader_tag().is_none());

            material_pool::texture_cache::add_image(internal::active_shader_tag(), image_id);
        }

        //-------------------------------------------------------------------------
        void reset_textures()
        {
            assert(!internal::active_shader_tag().is_none());

            material_pool::texture_cache::reset_images(internal::active_shader_tag());
        }

        //-------------------------------------------------------------------------
        void shader(std::string_view tag)
        {
            auto sid_tag = string::store_sid(tag);

            internal::active_shader_tag() = sid_tag;

            auto it = internal::shader_tag_vertex_type_map().find(sid_tag);
            auto vertex_type = it == std::cend(internal::shader_tag_vertex_type_map())
                ? render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR
                : it->second;

            render::push_active_shader(sid_tag, vertex_type);
        }

        //-------------------------------------------------------------------------
        shader_program normal_material()
        {
            string::string_id tag = render::draw_mode() == render::render_draw_mode::BATCHED 
                ? shader_pool::tags::unlit_normal()
                : shader_pool::tags::instance_unlit_normal();

            internal::active_shader_tag() = tag;

            render::push_active_shader(tag, internal::shader_tag_vertex_type_map().at(tag));

            return get_shader(string::restore_sid(tag));
        }

        //-------------------------------------------------------------------------
        shader_program specular_material()
        {
            string::string_id tag = render::draw_mode() == render::render_draw_mode::BATCHED
                ? shader_pool::tags::lit_specular()
                : shader_pool::tags::instance_lit_specular();

            internal::active_shader_tag() = tag;

            render::push_active_shader(tag, internal::shader_tag_vertex_type_map().at(tag));
            
            return get_shader(string::restore_sid(tag));
        }

        //-------------------------------------------------------------------------
        shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source)
        {
            auto sid_tag = string::store_sid(tag);

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, vertex_source, fragment_source);

            material_pool::add_new_material(resources::material(sid_tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, std::string_view geometry_source)
        {
            auto sid_tag = string::store_sid(tag);

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, vertex_source, fragment_source, geometry_source);

            material_pool::add_new_material(resources::material(sid_tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path)
        {
            auto sid_tag = string::store_sid(tag);

            if (shader_pool::has_shader(sid_tag))
            {
                return { shader_pool::get_shader_program(sid_tag) };
            }

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, vs_buffer.c_str(), fs_buffer.c_str());

            material_pool::add_new_material(resources::material(sid_tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, std::string_view geometry_path)
        {
            shader_program shader_program = get_shader(tag);
            if (shader_program.id != -1)
            {
                return shader_program;
            }

            auto sid_tag = string::store_sid(tag);

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto gs_buffer = fileio::read_text_file(geometry_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, vs_buffer.c_str(), fs_buffer.c_str(), gs_buffer.c_str());

            material_pool::add_new_material(resources::material(sid_tag));
                        
            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program get_shader(std::string_view tag)
        {
            auto sid_tag = string::store_sid(tag);

            if (material_pool::has_material(sid_tag))
            {
                auto material = material_pool::material_at_shader_tag(sid_tag);

                assert(shader_pool::has_shader(material->shader_tag()));

                return { shader_pool::get_shader_program(material->shader_tag()) };
            }

            return { -1u };
        }
    }
}