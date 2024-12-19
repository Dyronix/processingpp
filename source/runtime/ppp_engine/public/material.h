#pragma once

#include <glm/glm.hpp>

#include <string>

namespace ppp
{
    namespace material
    {
        namespace tags
        {
            const std::string& unlit_color();
            const std::string& unlit_texture();
            const std::string& unlit_font();
            const std::string& unlit_normal();

            const std::string& lit_specular();
        }

        using shader_program_id = unsigned int;

        struct shader_program
        {
            shader_program_id id;

            shader_program();
            shader_program(shader_program_id in_id);

            void set_uniform(const std::string& uniform_name, bool value);
            void set_uniform(const std::string& uniform_name, int value);
            void set_uniform(const std::string& uniform_name, float value);
            void set_uniform(const std::string& uniform_name, const glm::vec2& value);
            void set_uniform(const std::string& uniform_name, const glm::vec3& value);
            void set_uniform(const std::string& uniform_name, const glm::vec4& value);
            void set_uniform(const std::string& uniform_name, const glm::mat2& value);
            void set_uniform(const std::string& uniform_name, const glm::mat3& value);
            void set_uniform(const std::string& uniform_name, const glm::mat4& value);
        };

        void texture(unsigned int image_id);
        void texture(unsigned int image_id, unsigned int texture_channel);

        void reset_texture(unsigned int texture_channel = 0);
        void reset_textures();

        unsigned int get_texture(unsigned int texture_channel = 0);

        void shader(const std::string& tag);
        void reset_shader();

        shader_program normal_material();
        shader_program specular_material();

        shader_program create_shader(const std::string& tag, const std::string& vertex_source, const std::string& fragment_source);
        shader_program create_shader(const std::string& tag, const std::string& vertex_source, const std::string& fragment_source, const std::string& geometry_source);
        shader_program load_shader(const std::string& tag, const std::string& vertex_path, const std::string& fragment_path);
        shader_program load_shader(const std::string& tag, const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path);
        
        shader_program get_shader(const std::string& tag);
    }
}