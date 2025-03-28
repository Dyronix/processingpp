#pragma once

#include "string/string_id.h"

namespace ppp
{
    struct camera_context;

    namespace render
    {
        class batch_data_table;
        class instance_data_table;

        using instance_data_hash_map = std::unordered_map<string::string_id, std::unique_ptr<instance_data_table>>;
        using batch_data_hash_map = std::unordered_map<string::string_id, std::unique_ptr<batch_data_table>>;

        struct render_scissor;

        struct render_context
        {
            explicit operator bool() const
            {
                return camera_context != nullptr
                    && scissor != nullptr
                    && font_batch_data != nullptr
                    && batch_data != nullptr
                    && instance_data != nullptr;
            }

            const camera_context*       camera_context = nullptr; 
            const render_scissor*       scissor = nullptr;

            batch_data_table*           font_batch_data = nullptr;

            batch_data_hash_map*        batch_data = nullptr;
            instance_data_hash_map*     instance_data = nullptr;
        };
    }
}