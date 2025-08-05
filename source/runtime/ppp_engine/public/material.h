/**
 * @file material.h
 * @brief Material and shader management utilities for rendering.
 */
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
                /**
                 * @brief Get the shader tag for unlit color.
                 * @return String identifier of the unlit color shader.
                 */
                std::string_view color();
                /**
                 * @brief Get the shader tag for unlit texture.
                 * @return String identifier of the unlit texture shader.
                 */
                std::string_view texture();
                /**
                 * @brief Get the shader tag for unlit color ui.
                 * @return String identifier of the unlit color ui shader.
                 */
                std::string_view ui_color();
                /**
                 * @brief Get the shader tag for unlit ui texture.
                 * @return String identifier of the unlit ui texture shader.
                 */
                std::string_view ui_texture();
                /**
                 * @brief Get the shader tag for unlit font.
                 * @return String identifier of the unlit font shader.
                 */
                std::string_view font();
                /**
                 * @brief Get the shader tag for unlit normal visualization.
                 * @return String identifier of the unlit normal shader.
                 */
                std::string_view normal();
            }

            namespace lit
            {
                /**
                 * @brief Get the shader tag for lit color.
                 * @return String identifier of the lit color shader.
                 */
                std::string_view color();
                /**
                 * @brief Get the shader tag for lit texture.
                 * @return String identifier of the lit texture shader.
                 */
                std::string_view texture();
                /**
                 * @brief Get the shader tag for lit specular.
                 * @return String identifier of the lit specular shader.
                 */
                std::string_view specular();
            }
        }
    }

    /**
     * @brief Shading model to use for materials.
     */
    enum class shading_model : std::uint8_t
    {
        LIT,       
        UNLIT      
    };

    /**
     * @brief Blending mode for materials.
     */
    enum class shading_blending : std::uint8_t
    {
        OPAQUE,         
        TRANSPARENT     
    };

    /**
     * @brief Type alias for unique identifier for a shader program.
     */
    using shader_program_id = unsigned int;

    /**
     * @brief Represents a shader program and its uniforms.
     */
    struct shader_program
    {
        shader_program_id id; ///< Identifier of the shader program

        /** @brief Default constructor initializing id to invalid. */
        shader_program();
        /**
         * @brief Construct a shader_program from an existing id.
         * @param in_id Identifier of the shader program.
         */
        shader_program(shader_program_id in_id);

        /**
         * @brief Set a boolean uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value Boolean value to set.
         */
        void set_uniform(std::string_view uniform_name, bool value);
        /**
         * @brief Set an integer uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value Integer value to set.
         */
        void set_uniform(std::string_view uniform_name, int value);
        /**
         * @brief Set a float uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value Float value to set.
         */
        void set_uniform(std::string_view uniform_name, float value);
        /**
         * @brief Set a vec2 uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value 2D vector value to set.
         */
        void set_uniform(std::string_view uniform_name, const vec2& value);
        /**
         * @brief Set a vec3 uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value 3D vector value to set.
         */
        void set_uniform(std::string_view uniform_name, const vec3& value);
        /**
         * @brief Set a vec4 uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value 4D vector value to set.
         */
        void set_uniform(std::string_view uniform_name, const vec4& value);
        /**
         * @brief Set a mat2 uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value 2x2 matrix value to set.
         */
        void set_uniform(std::string_view uniform_name, const mat2& value);
        /**
         * @brief Set a mat3 uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value 3x3 matrix value to set.
         */
        void set_uniform(std::string_view uniform_name, const mat3& value);
        /**
         * @brief Set a mat4 uniform.
         * @param uniform_name Name of the uniform variable.
         * @param value 4x4 matrix value to set.
         */
        void set_uniform(std::string_view uniform_name, const mat4& value);
    };

    /**
     * @brief Bind an image texture to the current shader.
     * @param image_id Identifier of the image to bind.
     */
    void texture(unsigned int image_id);

    /**
     * @brief Reset all bound textures for the current shader.
     */
    void reset_textures();

    /**
     * @brief Activate a shader by its tag.
     * @param tag String tag of the shader to activate.
     */
    void shader(std::string_view tag);

    /**
     * @brief Use the default normal unlit material.
     * @return Shader program configured for unlit normal rendering.
     */
    shader_program normal_material();

    /**
     * @brief Use the default specular lit material.
     * @return Shader program configured for lit specular rendering.
     */
    shader_program specular_material();

    /**
     * @brief Create a new shader program from source strings.
     * @param tag Unique tag for the shader.
     * @param vertex_source GLSL source code for the vertex shader.
     * @param fragment_source GLSL source code for the fragment shader.
     * @param shading_model Shading model to use (lit or unlit).
     * @param shading_blend Blending mode for transparency.
     * @return Shader program representing the compiled shader.
     */
    shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);

    /**
     * @brief Create a new shader program with geometry shader support.
     * @param tag Unique tag for the shader.
     * @param vertex_source GLSL source code for the vertex shader.
     * @param fragment_source GLSL source code for the fragment shader.
     * @param geometry_source GLSL source code for the geometry shader.
     * @param shading_model Shading model to use.
     * @param shading_blend Blending mode for transparency.
     * @return Shader program representing the compiled shader.
     */
    shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, std::string_view geometry_source, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);

    /**
     * @brief Load or compile a shader program from file paths.
     * @param tag Unique tag for the shader.
     * @param vertex_path Filesystem path to the vertex shader file.
     * @param fragment_path Filesystem path to the fragment shader file.
     * @param shading_model Shading model to use.
     * @param shading_blend Blending mode.
     * @return Shader program representing the loaded shader.
     */
    shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);

    /**
     * @brief Load or compile a shader program with geometry shader support.
     * @param tag Unique tag for the shader.
     * @param vertex_path Filesystem path to the vertex shader file.
     * @param fragment_path Filesystem path to the fragment shader file.
     * @param geometry_path Filesystem path to the geometry shader file.
     * @param shading_model Shading model to use.
     * @param shading_blend Blending mode.
     * @return Shader program representing the loaded shader.
     */
    shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, std::string_view geometry_path, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);

    /**
     * @brief Retrieve a shader program by its tag.
     * @param tag String tag of the shader.
     * @return Shader program if found; otherwise invalid.
     */
    shader_program get_shader(std::string_view tag);

} // namespace ppp
