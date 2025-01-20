#include "material.h"
#include "fileio/fileio.h"
#include "render/render_shaders.h"
#include "render/render.h"
#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "resources/texture_manager.h"
#include "util/log.h"

namespace ppp
{
    namespace material
    {
        namespace tags
        {
            //-------------------------------------------------------------------------
            // batched
            const std::string& unlit_color()
            {
                return render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit_color
                    : shader_pool::tags::instance_unlit_color;
            }
            //-------------------------------------------------------------------------
            const std::string& unlit_texture()
            {
                return render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit_texture
                    : shader_pool::tags::instance_unlit_texture;
            }
            //-------------------------------------------------------------------------
            const std::string& unlit_font()
            {
                return shader_pool::tags::unlit_font;
            }
            //-------------------------------------------------------------------------
            const std::string& unlit_normal()
            {
                return render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::unlit_normal
                    : shader_pool::tags::instance_unlit_normal;
            }

            //-------------------------------------------------------------------------
            const std::string& lit_specular()
            {
                return render::draw_mode() == render::render_draw_mode::BATCHED
                    ? shader_pool::tags::lit_specular
                    : shader_pool::tags::instance_lit_specular;
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
        void shader_program::set_uniform(const std::string& uniform_name, bool value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, int value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, float value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, const glm::vec2& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, const glm::vec3& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, const glm::vec4& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, const glm::mat2& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, const glm::mat3& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        //-------------------------------------------------------------------------
        void shader_program::set_uniform(const std::string& uniform_name, const glm::mat4& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }

        //-------------------------------------------------------------------------
        void texture(unsigned int image_id)
        {
            assert(material_pool::active_material());

            texture_manager::add_image(material_pool::active_material()->shader_tag(), image_id);
        }

        //-------------------------------------------------------------------------
        void reset_textures()
        {
            assert(material_pool::active_material());

            texture_manager::reset_images(material_pool::active_material()->shader_tag());
        }

        //-------------------------------------------------------------------------
        void shader(const std::string& tag)
        {
            material_pool::set_active_material(tag);

            render::push_active_shader(tag, render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR);
        }
        
        //-------------------------------------------------------------------------
        void reset_shader()
        {
            material_pool::reset_active_material();

            render::push_reset_shader();
        }

        //-------------------------------------------------------------------------
        shader_program normal_material()
        {
            const std::string& tag = render::draw_mode() == render::render_draw_mode::BATCHED 
                ? shader_pool::tags::unlit_normal 
                : shader_pool::tags::instance_unlit_normal;

            material_pool::set_active_material(tag);

            render::vertex_type vertex_type = render::draw_mode() == render::render_draw_mode::BATCHED
                ? render::vertex_type::POSITION_NORMAL_COLOR
                : render::vertex_type::POSITION_NORMAL;

            render::push_active_shader(tag, vertex_type);

            return get_shader(tag);
        }

        //-------------------------------------------------------------------------
        shader_program specular_material()
        {
            const std::string& tag = render::draw_mode() == render::render_draw_mode::BATCHED
                ? shader_pool::tags::lit_specular
                : shader_pool::tags::instance_lit_specular;

            material_pool::set_active_material(tag);

            render::vertex_type vertex_type = render::draw_mode() == render::render_draw_mode::BATCHED
                ? render::vertex_type::POSITION_NORMAL_COLOR
                : render::vertex_type::POSITION_NORMAL;

            render::push_active_shader(tag, vertex_type);
            
            return get_shader(tag);
        }

        //-------------------------------------------------------------------------
        shader_program create_shader(const std::string& tag, const std::string& vertex_source, const std::string& fragment_source)
        {
            u32 shader_program_id = shader_pool::add_shader_program(tag, vertex_source.c_str(), fragment_source.c_str());

            material_pool::add_new_material(resources::material(tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program create_shader(const std::string& tag, const std::string& vertex_source, const std::string& fragment_source, const std::string& geometry_source)
        {
            u32 shader_program_id = shader_pool::add_shader_program(tag, vertex_source.c_str(), fragment_source.c_str(), geometry_source.c_str());

            material_pool::add_new_material(resources::material(tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program load_shader(const std::string& tag, const std::string& vertex_path, const std::string& fragment_path)
        {
            if (shader_pool::has_shader(tag))
            {
                return { shader_pool::get_shader_program(tag) };
            }

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(tag, vs_buffer.c_str(), fs_buffer.c_str());

            material_pool::add_new_material(resources::material(tag));

            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program load_shader(const std::string& tag, const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path)
        {
            shader_program shader_program = get_shader(tag);
            if (shader_program.id != -1)
            {
                return shader_program;
            }

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto gs_buffer = fileio::read_text_file(geometry_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(tag, vs_buffer.c_str(), fs_buffer.c_str(), gs_buffer.c_str());

            material_pool::add_new_material(resources::material(tag));
                        
            return { shader_program_id };
        }

        //-------------------------------------------------------------------------
        shader_program get_shader(const std::string& tag)
        {
            if (material_pool::has_material(tag))
            {
                auto material = material_pool::material_at_shader_tag(tag);

                assert(shader_pool::has_shader(material->shader_tag()));

                return { shader_pool::get_shader_program(material->shader_tag()) };
            }

            return { -1u };
        }
    }
}