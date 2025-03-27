#pragma once

#include "string/string_id.h"

namespace ppp
{
    struct camera_context;

    namespace render
    {
        class texture_batch_renderer;
        class instance_renderer;
        class batch_data_table;

        using instance_renderers_hash_map = std::unordered_map<string::string_id, std::unique_ptr<instance_renderer>>;
        using batch_data_hash_map = std::unordered_map<string::string_id, std::unique_ptr<batch_data_table>>;

        struct render_scissor;

        struct render_context
        {
            explicit operator bool() const
            {
                return camera_context != nullptr
                    && scissor != nullptr
                    && font_renderer != nullptr
                    && batch_data != nullptr
                    && instance_renderers != nullptr;
            }

            const camera_context*           camera_context = nullptr; 

            const render_scissor*           scissor = nullptr;

            texture_batch_renderer*         font_renderer = nullptr;
            batch_data_hash_map*            batch_data = nullptr;
            instance_renderers_hash_map*    instance_renderers = nullptr;
        };
    }
}