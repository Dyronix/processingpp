#include "material.h"
#include "fileio/fileio.h"
#include "render/render_shader_uniform_manager.h"
#include "render/render.h"
#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "util/log.h"


#include "string/string_id.h"

namespace ppp
{
    namespace material
    {
        namespace conversions
        {
            render::shading_model_type to_shading_model_type(shading_model shading_model)
            {
                switch (shading_model)
                {
                case shading_model::LIT: return render::shading_model_type::LIT;
                case shading_model::UNLIT: return render::shading_model_type::UNLIT;
                }

                log::error("Unknown shading model!");
                exit(EXIT_FAILURE);
            }
        }
        namespace tags
        {
            //-------------------------------------------------------------------------
            // unlit
            std::string_view unlit_color()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit::color()
                    : shader_pool::tags::unlit::instance_color();

                return string::restore_sid(sid);
            }
            //-------------------------------------------------------------------------
            std::string_view unlit_texture()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit::texture()
                    : shader_pool::tags::unlit::instance_texture();

                return string::restore_sid(sid);
            }
            //-------------------------------------------------------------------------
            std::string_view unlit_font()
            {
                auto sid = shader_pool::tags::unlit::font();

                return string::restore_sid(sid);
            }
            //-------------------------------------------------------------------------
            std::string_view unlit_normal()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit::normal()
                    : shader_pool::tags::unlit::instance_normal();

                return string::restore_sid(sid);
            }

            //-------------------------------------------------------------------------
            // lit
            std::string_view lit_color()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::lit::color()
                    : shader_pool::tags::lit::instance_color();

                return string::restore_sid(sid);
            }
            //-------------------------------------------------------------------------
            std::string_view lit_specular()
            {
                auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::lit::specular()
                    : shader_pool::tags::lit::instance_specular();

                return string::restore_sid(sid);
            }
        }

        struct context
        {
            context()
                : active_shader_tag(string::string_id::create_invalid())
            {
            }

            string::string_id                                           active_shader_tag;
        } g_ctx;

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
            assert(!g_ctx.active_shader_tag.is_none());

            material_pool::texture_cache::add_image(g_ctx.active_shader_tag, image_id);
        }

        //-------------------------------------------------------------------------
        void reset_textures()
        {
            assert(!g_ctx.active_shader_tag.is_none());

            material_pool::texture_cache::reset_images(g_ctx.active_shader_tag);
        }

        //-------------------------------------------------------------------------
        void shader(std::string_view tag)
        {
            auto sid_tag = string::store_sid(tag);

            g_ctx.active_shader_tag = sid_tag;

            render::push_active_shader(sid_tag);
        }

        //-------------------------------------------------------------------------
        shader_program normal_material()
        {
            string::string_id tag = render::draw_mode() == render::render_draw_mode::BATCHED 
                ? shader_pool::tags::unlit::normal()
                : shader_pool::tags::unlit::instance_normal();

            g_ctx.active_shader_tag = tag;

            render::push_active_shader(tag);

            return get_shader(string::restore_sid(tag));
        }

        //-------------------------------------------------------------------------
        shader_program specular_material()
        {
            string::string_id tag = render::draw_mode() == render::render_draw_mode::BATCHED
                ? shader_pool::tags::lit::specular()
                : shader_pool::tags::lit::instance_specular();

            g_ctx.active_shader_tag = tag;

            render::push_active_shader(tag);
            
            return get_shader(string::restore_sid(tag));
        }

        //-------------------------------------------------------------------------
        shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, shading_model shading_model)
        {
            auto sid_tag = string::store_sid(tag);
            auto vertex_format = shading_model == shading_model::LIT
                ? render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR
                : render::vertex_type::POSITION_TEXCOORD_COLOR;

            if (shader_pool::has_shader(sid_tag))
            {
                return { shader_pool::get_shader_program(sid_tag)->id() };
            }

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), vertex_format, vertex_source, fragment_source);

            material_pool::add_new_material(resources::material(sid_tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, std::string_view geometry_source, shading_model shading_model)
        {
            auto sid_tag = string::store_sid(tag);
            auto vertex_format = shading_model == shading_model::LIT 
                ? render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR
                : render::vertex_type::POSITION_TEXCOORD_COLOR;

            if (shader_pool::has_shader(sid_tag))
            {
                return { shader_pool::get_shader_program(sid_tag)->id() };
            }

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), vertex_format, vertex_source, fragment_source, geometry_source);

            material_pool::add_new_material(resources::material(sid_tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, shading_model shading_model)
        {
            auto sid_tag = string::store_sid(tag);
            auto vertex_format = shading_model == shading_model::LIT
                ? render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR
                : render::vertex_type::POSITION_TEXCOORD_COLOR;

            if (shader_pool::has_shader(sid_tag))
            {
                return { shader_pool::get_shader_program(sid_tag)->id() };
            }

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), vertex_format, vs_buffer.c_str(), fs_buffer.c_str());

            material_pool::add_new_material(resources::material(sid_tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, std::string_view geometry_path, shading_model shading_model)
        {
            auto sid_tag = string::store_sid(tag);
            auto vertex_format = shading_model == shading_model::LIT
                ? render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR
                : render::vertex_type::POSITION_TEXCOORD_COLOR;

            if (shader_pool::has_shader(sid_tag))
            {
                return { shader_pool::get_shader_program(sid_tag)->id() };
            }

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto gs_buffer = fileio::read_text_file(geometry_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), vertex_format, vs_buffer.c_str(), fs_buffer.c_str(), gs_buffer.c_str());

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

                return { shader_pool::get_shader_program(material->shader_tag())->id() };
            }

            return { -1u };
        }
    }
}