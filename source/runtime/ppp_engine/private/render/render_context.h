#pragma once

#include "string/string_id.h"

#include "util/hash.h"

namespace ppp
{
    struct camera_context;

    namespace render
    {
        struct instance_data_key;
        struct batch_data_key;

        struct render_scissor;

        class batch_data_table;
        class instance_data_table;

        using instance_data_hash_map = std::unordered_map<instance_data_key, std::unique_ptr<instance_data_table>>;
        using batch_data_hash_map = std::unordered_map<batch_data_key, std::unique_ptr<batch_data_table>>;

        struct batch_data_key
        {
            string::string_id shader_tag;
            shading_model_type shader_model_type;

            bool cast_shadows;

            // Equality operator for comparisons.
            bool operator==(const batch_data_key& other) const
            {
                return shader_tag == other.shader_tag && cast_shadows == other.cast_shadows;
            }
        };

        struct instance_data_key
        {
            string::string_id shader_tag;
            shading_model_type shader_model_type;

            bool cast_shadows;

            // Equality operator for comparisons.
            bool operator==(const instance_data_key& other) const
            {
                return shader_tag == other.shader_tag && cast_shadows == other.cast_shadows;
            }
        };

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

namespace std
{
    template<>
    struct hash<ppp::render::batch_data_key>
    {
        std::size_t operator()(const ppp::render::batch_data_key& key) const noexcept
        {
            size_t seed = 0;
            seed = ppp::utils::hash_combine(seed, key.shader_tag);
            seed = ppp::utils::hash_combine(seed, key.shader_model_type);
            seed = ppp::utils::hash_combine(seed, key.cast_shadows);
            return seed;
        }
    };

    template<>
    struct hash<ppp::render::instance_data_key>
    {
        std::size_t operator()(const ppp::render::instance_data_key& key) const noexcept
        {
            size_t seed = 0;
            seed = ppp::utils::hash_combine(seed, key.shader_tag);
            seed = ppp::utils::hash_combine(seed, key.shader_model_type);
            seed = ppp::utils::hash_combine(seed, key.cast_shadows);
            return seed;
        }
    };
}