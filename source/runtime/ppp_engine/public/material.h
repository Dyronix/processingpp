#pragma once

#include <glm/glm.hpp>

#include <string>

namespace ppp
{
    namespace material
    {
        namespace tags
        {
            std::string_view unlit_color();
            std::string_view unlit_texture();
            std::string_view unlit_font();
            std::string_view unlit_normal();

            std::string_view lit_specular();
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
        void reset_textures();

        void shader(std::string_view tag);

        shader_program normal_material();
        shader_program specular_material();

        shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source);
        shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, std::string_view geometry_source);
        shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path);
        shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, std::string_view geometry_path);
        
        shader_program get_shader(std::string_view tag);
    }
}