#include "resources/shader_pool.h"

#include "render/render_shader_compiler.h"
#include "render/render_shader_library.h"
#include "render/render_shader_tags.h"

#include "util/log.h"

#include <unordered_map>

namespace ppp
{
    namespace shader_pool
    {
        using shaders_hash_map = std::unordered_map<string::string_id, std::shared_ptr<render::shaders::shader_program>>;

        struct context
        {
            //-------------------------------------------------------------------------
            void add_shader_program(string::string_id sid, render::shading_model_type smtype, render::shading_blending_type btype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source)
            {
                shaders_hash_map.emplace(sid, std::make_shared<render::shaders::shader_program>(smtype, btype, vtype, vs_source, fs_source));
            }

            //-------------------------------------------------------------------------
            void add_shader_program(string::string_id sid, render::shading_model_type smtype, render::shading_blending_type btype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source)
            {
                shaders_hash_map.emplace(sid, std::make_shared<render::shaders::shader_program>(smtype, btype, vtype, vs_source, fs_source, gs_source));
            }

            shaders_hash_map shaders_hash_map;
        } g_ctx;

        //-------------------------------------------------------------------------
        bool initialize()
        {
            // batched
            // unlit
            g_ctx.add_shader_program(render::unlit::tags::color::batched(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_COLOR, render::shaders::unlit::color_vertex_shader_code(), render::shaders::unlit::color_pixel_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::texture::batched(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_TEXCOORD_COLOR, render::shaders::unlit::texture_vertex_shader_code(), render::shaders::unlit::texture_pixel_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::font::batched(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_TEXCOORD_COLOR, render::shaders::unlit::font_vertex_shader_code(), render::shaders::unlit::font_pixel_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::normal::batched(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_NORMAL_COLOR, render::shaders::unlit::normal_vertex_shader_code(), render::shaders::unlit::normal_pixel_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::shadow::batched(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION, render::shaders::unlit::shadow_depth_vertex_shader_code(), render::shaders::unlit::shadow_depth_fragment_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::predepth::batched(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION, render::shaders::unlit::predepth_vertex_shader_code(), render::shaders::unlit::predepth_fragment_shader_code());
            // lit
            g_ctx.add_shader_program(render::lit::tags::color::batched(), render::shading_model_type::LIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_NORMAL_COLOR, render::shaders::lit::color_vertex_shader_code(), render::shaders::lit::color_pixel_shader_code());
            g_ctx.add_shader_program(render::lit::tags::texture::batched(), render::shading_model_type::LIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR, render::shaders::lit::texture_vertex_shader_code(), render::shaders::lit::texture_pixel_shader_code());
            g_ctx.add_shader_program(render::lit::tags::specular::batched(), render::shading_model_type::LIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_NORMAL_COLOR, render::shaders::lit::specular_vertex_shader_code(), render::shaders::lit::specular_pixel_shader_code());

            // instanced
            // unlit
            g_ctx.add_shader_program(render::unlit::tags::color::instanced(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION, render::shaders::unlit::instance_color_vertex_shader_code(), render::shaders::unlit::color_pixel_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::texture::instanced(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_TEXCOORD, render::shaders::unlit::instance_texture_vertex_shader_code(), render::shaders::unlit::texture_pixel_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::normal::instanced(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_NORMAL, render::shaders::unlit::instance_normal_vertex_shader_code(), render::shaders::unlit::normal_pixel_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::shadow::instanced(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION, render::shaders::unlit::instance_shadow_depth_vertex_shader_code(), render::shaders::unlit::shadow_depth_fragment_shader_code());
            g_ctx.add_shader_program(render::unlit::tags::predepth::instanced(), render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION, render::shaders::unlit::instance_predepth_vertex_shader_code(), render::shaders::unlit::predepth_fragment_shader_code());
            // lit
            g_ctx.add_shader_program(render::lit::tags::color::instanced(), render::shading_model_type::LIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_NORMAL, render::shaders::lit::instance_color_vertex_shader_code(), render::shaders::lit::color_pixel_shader_code());
            g_ctx.add_shader_program(render::lit::tags::texture::instanced(), render::shading_model_type::LIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR, render::shaders::lit::instance_texture_vertex_shader_code(), render::shaders::lit::texture_pixel_shader_code());
            g_ctx.add_shader_program(render::lit::tags::specular::instanced(), render::shading_model_type::LIT, render::shading_blending_type::OPAQUE, render::vertex_type::POSITION_NORMAL, render::shaders::lit::instance_specular_vertex_shader_code(), render::shaders::lit::specular_pixel_shader_code());

            bool invalid_shader_found = std::any_of(std::cbegin(g_ctx.shaders_hash_map), std::cend(g_ctx.shaders_hash_map),
                [](const auto& pair)
            {
                if (pair.second->id() == -1)
                {
                    log::error("Failed shader: {}", string::restore_sid(pair.first));
                    return true;
                }

                return false;
            });

            if (invalid_shader_found)
            {
                log::error("One or more shaders failed to compile");
                return false;
            }

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
        u32 add_shader_program(string::string_id tag, render::shading_model_type smtype, render::shading_blending_type btype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            if(it == std::cend(g_ctx.shaders_hash_map))
            {
                auto shader_program = std::make_shared<render::shaders::shader_program>(smtype, btype, vtype, vs_source, fs_source);
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
        u32 add_shader_program(string::string_id tag, render::shading_model_type smtype, render::shading_blending_type btype, render::vertex_type vtype, std::string_view vs_source, std::string_view fs_source, std::string_view gs_source)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            if (it == std::cend(g_ctx.shaders_hash_map))
            {
                auto shader_program = std::make_shared<render::shaders::shader_program>(smtype, btype, vtype, vs_source, fs_source, gs_source);
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

        //-------------------------------------------------------------------------
        render::shading_model_type shading_model_for_shader(string::string_id tag)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            if (it != std::cend(g_ctx.shaders_hash_map))
            {
                return it->second->shading_model();
            }

            log::error("Unable to find shader with tag: {}", string::restore_sid(tag));
            return render::shading_model_type::UNLIT;
        }

        //-------------------------------------------------------------------------
        render::shading_blending_type shading_blending_for_shader(string::string_id tag)
        {
            auto it = g_ctx.shaders_hash_map.find(tag);
            if (it != std::cend(g_ctx.shaders_hash_map))
            {
                return it->second->shading_blending();
            }

            log::error("Unable to find shader with tag: {}", string::restore_sid(tag));
            return render::shading_blending_type::OPAQUE;
        }
    }
}