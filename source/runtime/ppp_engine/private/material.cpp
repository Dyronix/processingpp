#include "material.h"
#include "fileio/fileio.h"
#include "render/render_shader_uniform_manager.h"
#include "render/render_shader_tags.h"
#include "render/render.h"
#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "util/log.h"

#include "string/string_id.h"

namespace ppp
{
    namespace material
    {
        namespace tags
        {
            namespace unlit
            {
                //-------------------------------------------------------------------------
                // unlit
                std::string_view color()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::unlit::tags::color::batched()
                        : render::unlit::tags::color::instanced();

                    return string::restore_sid(sid);
                }
                //-------------------------------------------------------------------------
                std::string_view texture()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::unlit::tags::texture::batched()
                        : render::unlit::tags::texture::instanced();

                    return string::restore_sid(sid);
                }
                //-------------------------------------------------------------------------
                std::string_view ui_color()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::unlit::tags::ui_color::batched()
                        : render::unlit::tags::ui_color::instanced();

                    return string::restore_sid(sid);
                }
                //-------------------------------------------------------------------------
                std::string_view ui_texture()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::unlit::tags::ui_texture::batched()
                        : render::unlit::tags::ui_texture::instanced();

                    return string::restore_sid(sid);
                }
                //-------------------------------------------------------------------------
                std::string_view font()
                {
                    auto sid = render::unlit::tags::font::batched();

                    return string::restore_sid(sid);
                }
                //-------------------------------------------------------------------------
                std::string_view normal()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::unlit::tags::normal::batched()
                        : render::unlit::tags::normal::instanced();

                    return string::restore_sid(sid);
                }
            }

            namespace lit
            {
                //-------------------------------------------------------------------------
                // lit
                std::string_view color()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::lit::tags::color::batched()
                        : render::lit::tags::color::instanced();

                    return string::restore_sid(sid);
                }
                std::string_view texture()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::lit::tags::texture::batched()
                        : render::lit::tags::texture::instanced();

                    return string::restore_sid(sid);
                }
                //-------------------------------------------------------------------------
                std::string_view specular()
                {
                    auto sid = render::draw_mode() == render::render_draw_mode::BATCHED
                        ? render::lit::tags::specular::batched()
                        : render::lit::tags::specular::instanced();

                    return string::restore_sid(sid);
                }
            }
        }
    }

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

        render::shading_blending_type to_shading_blending_type(shading_blending shading_blending)
        {
            switch (shading_blending)
            {
            case shading_blending::OPAQUE: return render::shading_blending_type::OPAQUE;
            case shading_blending::TRANSPARENT: return render::shading_blending_type::TRANSPARENT;
            case shading_blending::UI: return render::shading_blending_type::UI;
            }

            log::error("Unknown shading blending!");
            exit(EXIT_FAILURE);
        }
    }

    struct context
    {
        context()
            : active_shader_tag(string::string_id::create_invalid())
        {}

        string::string_id active_shader_tag;
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
    void shader_program::set_uniform(std::string_view uniform_name, const vec2& value)
    {
        render::shaders::push_uniform(id, string::store_sid(uniform_name), glm::vec2{ value.x, value.y });
    }
    //-------------------------------------------------------------------------
    void shader_program::set_uniform(std::string_view uniform_name, const vec3& value)
    {
        render::shaders::push_uniform(id, string::store_sid(uniform_name), glm::vec3{ value.x, value.y, value.z });
    }
    //-------------------------------------------------------------------------
    void shader_program::set_uniform(std::string_view uniform_name, const vec4& value)
    {
        render::shaders::push_uniform(id, string::store_sid(uniform_name), glm::vec4{ value.x, value.y, value.z, value.w });
    }
    //-------------------------------------------------------------------------
    void shader_program::set_uniform(std::string_view uniform_name, const mat2& value)
    {
        render::shaders::push_uniform(id, string::store_sid(uniform_name), reinterpret_cast<const glm::mat2&>(value));
    }
    //-------------------------------------------------------------------------
    void shader_program::set_uniform(std::string_view uniform_name, const mat3& value)
    {
        render::shaders::push_uniform(id, string::store_sid(uniform_name), reinterpret_cast<const glm::mat3&>(value));
    }
    //-------------------------------------------------------------------------
    void shader_program::set_uniform(std::string_view uniform_name, const mat4& value)
    {
        render::shaders::push_uniform(id, string::store_sid(uniform_name), reinterpret_cast<const glm::mat4&>(value));
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

        render::push_active_shader(sid_tag, shader_pool::shading_model_for_shader(sid_tag), shader_pool::shading_blending_for_shader(sid_tag));
    }

    //-------------------------------------------------------------------------
    shader_program normal_material()
    {
        string::string_id tag = render::draw_mode() == render::render_draw_mode::BATCHED
            ? render::unlit::tags::normal::batched()
            : render::unlit::tags::normal::instanced();

        g_ctx.active_shader_tag = tag;

        render::push_active_shader(tag, render::shading_model_type::UNLIT, render::shading_blending_type::OPAQUE);

        return get_shader(string::restore_sid(tag));
    }

    //-------------------------------------------------------------------------
    shader_program specular_material()
    {
        string::string_id tag = render::draw_mode() == render::render_draw_mode::BATCHED
            ? render::lit::tags::specular::batched()
            : render::lit::tags::specular::instanced();

        g_ctx.active_shader_tag = tag;

        render::push_active_shader(tag, render::shading_model_type::LIT, render::shading_blending_type::OPAQUE);

        return get_shader(string::restore_sid(tag));
    }

    //-------------------------------------------------------------------------
    shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, shading_model shading_model, shading_blending shading_blend)
    {
        auto sid_tag = string::store_sid(tag);
        auto vertex_format = shading_model == shading_model::LIT
            ? render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR
            : render::vertex_type::POSITION_TEXCOORD_COLOR;

        if (shader_pool::has_shader(sid_tag))
        {
            return { shader_pool::get_shader_program(sid_tag)->id() };
        }

        u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), conversions::to_shading_blending_type(shading_blend), vertex_format, vertex_source, fragment_source);

        material_pool::add_new_material(resources::material(sid_tag));

        return { shader_program_id };
    }

    //-------------------------------------------------------------------------
    shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, std::string_view geometry_source, shading_model shading_model, shading_blending shading_blend)
    {
        auto sid_tag = string::store_sid(tag);
        auto vertex_format = shading_model == shading_model::LIT
            ? render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR
            : render::vertex_type::POSITION_TEXCOORD_COLOR;

        if (shader_pool::has_shader(sid_tag))
        {
            return { shader_pool::get_shader_program(sid_tag)->id() };
        }

        u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), conversions::to_shading_blending_type(shading_blend), vertex_format, vertex_source, fragment_source, geometry_source);

        material_pool::add_new_material(resources::material(sid_tag));

        return { shader_program_id };
    }

    //-------------------------------------------------------------------------
    shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, shading_model shading_model, shading_blending shading_blend)
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

        u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), conversions::to_shading_blending_type(shading_blend), vertex_format, vs_buffer.c_str(), fs_buffer.c_str());

        material_pool::add_new_material(resources::material(sid_tag));

        return { shader_program_id };
    }

    //-------------------------------------------------------------------------
    shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, std::string_view geometry_path, shading_model shading_model, shading_blending shading_blend)
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

        u32 shader_program_id = shader_pool::add_shader_program(sid_tag, conversions::to_shading_model_type(shading_model), conversions::to_shading_blending_type(shading_blend), vertex_format, vs_buffer.c_str(), fs_buffer.c_str(), gs_buffer.c_str());

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