#pragma once

#include "vector.h"
#include "matrix.h"

#include <string>

namespace ppp
{
    namespace material
    {
        namespace tags
        {
            namespace unlit
            {
                std::string_view color();
                std::string_view texture();
                std::string_view font();
                std::string_view normal();
            }

            namespace lit
            {
                std::string_view color();
                std::string_view texture();
                std::string_view specular();
            }
        }
    }

    enum class shading_model
    {
        LIT,
        UNLIT
    };

    enum class shading_blending
    {
        OPAQUE,
        TRANSPARENT
    };

    using shader_program_id = unsigned int;

    struct shader_program
    {
        shader_program_id id;

        shader_program();
        shader_program(shader_program_id in_id);

        void set_uniform(std::string_view uniform_name, bool value);
        void set_uniform(std::string_view uniform_name, int value);
        void set_uniform(std::string_view uniform_name, float value);
        void set_uniform(std::string_view uniform_name, const vec2& value);
        void set_uniform(std::string_view uniform_name, const vec3& value);
        void set_uniform(std::string_view uniform_name, const vec4& value);
        void set_uniform(std::string_view uniform_name, const mat2& value);
        void set_uniform(std::string_view uniform_name, const mat3& value);
        void set_uniform(std::string_view uniform_name, const mat4& value);
    };

    void texture(unsigned int image_id);
    void reset_textures();

    void shader(std::string_view tag);

    shader_program normal_material();
    shader_program specular_material();

    shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);
    shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, std::string_view geometry_source, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);
    shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);
    shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, std::string_view geometry_path, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);

    shader_program get_shader(std::string_view tag);
}