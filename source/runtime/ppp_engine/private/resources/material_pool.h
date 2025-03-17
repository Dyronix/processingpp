#pragma once

#include "util/types.h"

#include "resources/material.h"



namespace ppp
{
    namespace material_pool
    {
        namespace texture_cache
        {
            void add_image(string::string_id shader_tag, render::texture_id image);
            void reset_images(string::string_id shader_tag);

            const std::vector<render::texture_id>& images(string::string_id shader_tag);
        }

        bool initialize();
        void terminate();

        bool has_material(string::string_id shader_tag);

        resources::imaterial* material_at_shader_tag(string::string_id shader_tag);
        resources::imaterial* material_instance_at_shader_tag(string::string_id shader_tag);

        void add_new_material(const resources::material& material);

        resources::imaterial* get_or_create_material_instance(string::string_id shader_tag);
    }
}