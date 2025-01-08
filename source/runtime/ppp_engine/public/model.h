#pragma once

#include <string>
#include <functional>

namespace ppp
{
    namespace model
    {
        enum class model_file_type
        {
            OBJ,
            STL
        };

        using model_id = unsigned long long;

        model_id load_model(const std::string& model_path);
        model_id create_model(const std::string& model_string, model_file_type file_type);

        void draw(model_id model_id);
    }
}