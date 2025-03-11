#pragma once

#include "memory/memory_types.h"
#include "memory/memory_unique_ptr_util.h"

#include "string/string_id.h"

namespace ppp
{
    namespace camera
    {
        struct camera_context;
    }

    namespace render
    {
        using instance_renderers_hash_map = graphics_hash_map<string::string_id, graphics_unique_ptr<instance_renderer>>;
        using batch_renderers_hash_map = graphics_hash_map<string::string_id, graphics_unique_ptr<batch_renderer>>;

        struct render_scissor;

        struct render_context
        {
            explicit operator bool() const
            {
                return camera_context != nullptr
                    && scissor != nullptr
                    && batch_renderers != nullptr
                    && instance_renderers != nullptr
            }

            camera::camera_context*         camera_context = nullptr; 

            render_scissor*                 scissor = nullptr;

            batch_renderers_hash_map*       batch_renderers = nullptr;
            instance_renderers_hash_map*    instance_renderers = nullptr;
        };
    }
}