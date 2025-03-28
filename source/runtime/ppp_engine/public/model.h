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
        };

        using model_id = unsigned long long;

        model_id load_model(std::string_view model_path);
        model_id create_model(std::string_view model_string, model_file_type file_type);

        void draw(model_id model_id);
    }
}