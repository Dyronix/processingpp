#include "resources/shader_pool.h"
#include "render/render_shader_compiler.h"
#include "render/render_shader_library.h"




#include "util/log.h"

#include <glad/glad.h>

#include <unordered_map>

namespace ppp
{
    namespace shader_pool
    {
        namespace tags
        {
            namespace unlit
            {
                //-------------------------------------------------------------------------
                // batched
                string::string_id color()
                {
                    static const string::string_id s_color = string::store_sid("unlit_color");

                    return s_color;
                }
                //-------------------------------------------------------------------------
                string::string_id texture()
                {
                    static const string::string_id s_texture = string::store_sid("unlit_texture");

                    return s_texture;
                }
                //-------------------------------------------------------------------------
                string::string_id font()
                {
                    static const string::string_id s_font = string::store_sid("unlit_font");

                    return s_font;
                }
                //-------------------------------------------------------------------------
                string::string_id normal()
                {
                    static const string::string_id s_normal = string::store_sid("unlit_normal");

                    return s_normal;
                }
                //-------------------------------------------------------------------------
                string::string_id shadow()
                {
                    static const string::string_id s_shadow = string::store_sid("unlit_shadow");

                    return s_shadow;
                }

                //-------------------------------------------------------------------------
                // instanced
                string::string_id instance_color()
                {
                    static const string::string_id s_instance_color = string::store_sid("instance_unlit_color");

                    return s_instance_color;
                }
                //-------------------------------------------------------------------------
                string::string_id instance_texture()
                {
                    static const string::string_id s_instance_texture = string::store_sid("instance_unlit_texture");

                    return s_instance_texture;
                }
                //-------------------------------------------------------------------------
                string::string_id instance_normal()
                {
                    static const string::string_id s_instance_normal = string::store_sid("instance_unlit_normal");

                    return s_instance_normal;
                }
                //-------------------------------------------------------------------------
                string::string_id instance_shadow()
                {
                    static const string::string_id s_instance_shadow = string::store_sid("instance_unlit_shadow");

                    return s_instance_shadow;
                }
            }

            namespace lit
            {
                //-------------------------------------------------------------------------
                // batched
                string::string_id color()
                {
                    static const string::string_id s_color = string::store_sid("lit_color");

                    return s_color;
                }
                string::string_id specular()
                {
                    static const string::string_id s_specular = string::store_sid("lit_specular");

                    return s_specular;
                }

                //-------------------------------------------------------------------------
                // instanced
                string::string_id instance_color()
                {
                    static const string::string_id s_instance_color = string::store_sid("instance_lit_color");

                    return s_instance_color;
                }
                string::string_id instance_specular()
                {
                    static const string::string_id s_instance_specular = string::store_sid("instance_lit_specular");

                    return s_instance_specular;
                }
            }
        }

        using shaders_hash_map = std::unordered_map<string::string_id, std::shared_ptr<render::shaders::shader_program>>;

        struct context
        {
            //-------------------------------------------------------------------------
            void add_shader_program(string::string_id sid, render::shading_model_type smtype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source)
            {
                shaders_hash_map.emplace(sid, std::make_shared<render::shaders::shader_program>(smtype, vtype, vs_source, fs_source));
            }

            //-------------------------------------------------------------------------
            void add_shader_program(string::string_id sid, render::shading_model_type smtype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source)
            {
                shaders_hash_map.emplace(sid, std::make_shared<render::shaders::shader_program>(smtype, vtype, vs_source, fs_source, gs_source));
            }

            shaders_hash_map shaders_hash_map;
        } g_ctx;

        //-------------------------------------------------------------------------
        bool initialize()
        {
            // batched
            // unlit
            g_ctx.add_shader_program(tags::unlit::color(), render::shading_model_type::UNLIT, render::vertex_type::POSITION_COLOR, render::shaders::unlit::color_vertex_shader_code(), render::shaders::unlit::color_pixel_shader_code());
            g_ctx.add_shader_program(tags::unlit::texture(), render::shading_model_type::UNLIT, render::vertex_type::POSITION_TEXCOORD_COLOR, render::shaders::unlit::texture_vertex_shader_code(), render::shaders::unlit::texture_pixel_shader_code());
            g_ctx.add_shader_program(tags::unlit::font(), render::shading_model_type::UNLIT, render::vertex_type::POSITION_TEXCOORD_COLOR, render::shaders::unlit::font_vertex_shader_code(), render::shaders::unlit::font_pixel_shader_code());
            g_ctx.add_shader_program(tags::unlit::normal(), render::shading_model_type::UNLIT, render::vertex_type::POSITION_NORMAL_COLOR, render::shaders::unlit::normal_vertex_shader_code(), render::shaders::unlit::normal_pixel_shader_code());
            g_ctx.add_shader_program(tags::unlit::shadow(), render::shading_model_type::UNLIT, render::vertex_type::POSITION, render::shaders::unlit::shadow_depth_vertex_shader_code(), render::shaders::unlit::shadow_depth_fragment_shader_code());
            // lit
            g_ctx.add_shader_program(tags::lit::color(), render::shading_model_type::LIT, render::vertex_type::POSITION_NORMAL_COLOR, render::shaders::lit::color_vertex_shader_code(), render::shaders::lit::color_pixel_shader_code());
            g_ctx.add_shader_program(tags::lit::specular(), render::shading_model_type::LIT, render::vertex_type::POSITION_NORMAL_COLOR, render::shaders::lit::specular_vertex_shader_code(), render::shaders::lit::specular_pixel_shader_code());

            // instanced
            // unlit
            g_ctx.add_shader_program(tags::unlit::instance_color(), render::shading_model_type::UNLIT, render::vertex_type::POSITION, render::shaders::unlit::instance_color_vertex_shader_code(), render::shaders::unlit::color_pixel_shader_code());
            g_ctx.add_shader_program(tags::unlit::instance_texture(), render::shading_model_type::UNLIT, render::vertex_type::POSITION_TEXCOORD, render::shaders::unlit::instance_texture_vertex_shader_code(), render::shaders::unlit::texture_pixel_shader_code());
            g_ctx.add_shader_program(tags::unlit::instance_normal(), render::shading_model_type::UNLIT, render::vertex_type::POSITION_NORMAL, render::shaders::unlit::instance_normal_vertex_shader_code(), render::shaders::unlit::normal_pixel_shader_code());
            g_ctx.add_shader_program(tags::unlit::instance_shadow(), render::shading_model_type::UNLIT, render::vertex_type::POSITION, render::shaders::unlit::instance_shadow_depth_vertex_shader_code(), render::shaders::unlit::shadow_depth_fragment_shader_code());
            // lit
            //g_ctx.add_shader_program(tags::lit::instance_color(), render::shading_model_type::LIT, render::vertex_type::POSITION_NORMAL, render::shaders::lit::instance_color_vertex_shader_code(), render::shaders::lit::color_pixel_shader_code());
            g_ctx.add_shader_program(tags::lit::instance_specular(), render::shading_model_type::LIT, render::vertex_type::POSITION_NORMAL, render::shaders::lit::instance_specular_vertex_shader_code(), render::shaders::lit::specular_pixel_shader_code());

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            g_ctx.shaders_hash_map.clear();
        }

        //-------------------------------------------------------------------------
        bool has_shader(string::string_id tag)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            return it != std::cend(g_ctx.shaders_hash_map);
        }

        //-------------------------------------------------------------------------
        u32 add_shader_program(string::string_id tag, render::shading_model_type smtype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            if(it == std::cend(g_ctx.shaders_hash_map))
            {
                auto shader_program = std::make_shared<render::shaders::shader_program>(smtype, vtype, vs_source, fs_source);
                auto shader_program_id = shader_program->id();
                g_ctx.shaders_hash_map.emplace(tag, shader_program);
                return shader_program_id;
            }
            else
            {
                log::warn("shader tag {} already exists", string::restore_sid(tag));
            }

            return -1;
        }

        //-------------------------------------------------------------------------
        u32 add_shader_program(string::string_id tag, render::shading_model_type smtype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            if (it == std::cend(g_ctx.shaders_hash_map))
            {
                auto shader_program = std::make_shared<render::shaders::shader_program>(smtype, vtype, vs_source, fs_source, gs_source);
                auto shader_program_id = shader_program->id();
                g_ctx.shaders_hash_map.emplace(tag, shader_program);
                return shader_program_id;
            }
            else
            {
                log::warn("shader tag {} already exists", string::restore_sid(tag));
            }

            return -1;
        }

        //-------------------------------------------------------------------------
        std::shared_ptr<render::shaders::shader_program> get_shader_program(string::string_id tag)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            if (it != std::cend(g_ctx.shaders_hash_map))
            {
                return it->second;
            }

            log::error("Unable to find shader with tag: {}", string::restore_sid(tag));
            return nullptr;
        }
    }
}