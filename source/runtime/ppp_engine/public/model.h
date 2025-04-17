/**
 * @file model.h
 * @brief Model loading and drawing utilities for in-memory and file-based models.
 */
#pragma once

#include <string>
#include <functional>

namespace ppp
{
    /**
     * @enum model_file_type
     * @brief Supported model file types.
     */
    enum class model_file_type : std::uint8_t
    {
        OBJ, ///< Wavefront OBJ format
    };

    /**
     * @brief Unique identifier for a model.
     */
    using model_id = unsigned long long;

    /**
     * @brief Create a model from in-memory model data.
     * @param model_string String containing the model file contents.
     * @param file_type Type of the model file (e.g., OBJ).
     * @return Identifier of the newly created model.
     */
    model_id create_model(std::string_view model_string, model_file_type file_type);

    /**
     * @brief Load a model from a file on disk.
     * @param model_path Filesystem path to the model file.
     * @return Identifier of the loaded model.
     */
    model_id load_model(std::string_view model_path);

    /**
     * @brief Render a loaded model.
     * @param model_id Identifier of the model to render.
     */
    void draw(model_id model_id);

} // namespace ppp
