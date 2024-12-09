#include "material.h"
#include "fileio/fileio.h"
#include "render/render_shaders.h"
#include "render/render.h"
#include "resources/shader_pool.h"
#include "util/log.h"

namespace ppp
{
    namespace material
    {
        shader_program::shader_program()
            :id(-1)
        {}

        shader_program::shader_program(shader_program_id in_id)
            :id(in_id)
        {}

        void shader_program::set_uniform(const std::string& uniform_name, bool value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, int value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, float value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, const glm::vec2& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, const glm::vec3& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, const glm::vec4& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, const glm::mat2& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, const glm::mat3& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }
        void shader_program::set_uniform(const std::string& uniform_name, const glm::mat4& value)
        {
            render::shaders::push_uniform(id, uniform_name, value);
        }

        void texture(unsigned int image_id)
        {
            texture(image_id, 0);
        }

        void texture(unsigned int image_id, unsigned int texture_channel)
        {

        }

        void shader(const std::string& tag)
        {
            render::push_active_shader(tag, render::vertex_type::POSITION_TEXCOORD_NORMAL_COLOR);
        }
        
        void reset()
        {
            render::push_reset_shader();
        }

        shader_program normal_material()
        {
            shader(shader_pool::tags::unlit_normal);

            return get_shader(shader_pool::tags::unlit_normal);
        }

        shader_program specular_material()
        {
            shader(shader_pool::tags::lit_specular);
            
            return get_shader(shader_pool::tags::lit_specular);
        }

        shader_program create_shader(const std::string& tag, const std::string& vertex_source, const std::string& fragment_source)
        {
            u32 shader_program_id = shader_pool::add_shader_program(tag, vertex_source.c_str(), fragment_source.c_str());
            return { shader_program_id };
        }

        shader_program create_shader(const std::string& tag, const std::string& vertex_source, const std::string& fragment_source, const std::string& geometry_source)
        {
            u32 shader_program_id = shader_pool::add_shader_program(tag, vertex_source.c_str(), fragment_source.c_str(), geometry_source.c_str());
            return { shader_program_id };
        }

        shader_program load_shader(const std::string& tag, const std::string& vertex_path, const std::string& fragment_path)
        {
            if (shader_pool::has_shader(tag))
            {
                return { shader_pool::get_shader_program(tag) };
            }

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(tag, vs_buffer.c_str(), fs_buffer.c_str());
            return { shader_program_id };
        }

        shader_program load_shader(const std::string& tag, const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path)
        {
            if (shader_pool::has_shader(tag))
            {
                return { shader_pool::get_shader_program(tag) };
            }

            auto vs_buffer = fileio::read_text_file(vertex_path);
            auto gs_buffer = fileio::read_text_file(geometry_path);
            auto fs_buffer = fileio::read_text_file(fragment_path);

            u32 shader_program_id = shader_pool::add_shader_program(tag, vs_buffer.c_str(), fs_buffer.c_str(), gs_buffer.c_str());
            return { shader_program_id };
        }

        shader_program get_shader(const std::string& tag)
        {
            if (shader_pool::has_shader(tag))
            {
                return { shader_pool::get_shader_program(tag) };
            }

            return { -1u };
        }
    }
}