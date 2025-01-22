#pragma once

#include "util/types.h"

#include "resources/material.h"

namespace ppp
{
    namespace material_pool
    {
        namespace texture_cache
        {
            void add_image(const std::string& shader_tag, render::texture_id image);
            void reset_images(const std::string& shader_tag);

            const std::vector<render::texture_id>& images(const std::string& shader_tag);
        }

        bool initialize();
        void terminate();

        bool has_material(const std::string& shader_tag);
        bool has_material(u64 id);

        resources::imaterial* material_at_shader_tag(const std::string& shader_tag);
        resources::imaterial* material_at_id(u64 id);

        resources::imaterial* material_instance_at_shader_tag(const std::string& shader_tag);
        resources::imaterial* material_instance_at_id(u64 id);

        void add_new_material(const resources::material& material);

        resources::imaterial* get_or_create_material_instance(const std::string& shader_tag);
    }
}