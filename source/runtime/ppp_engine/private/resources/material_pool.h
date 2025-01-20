#pragma once

#include "util/types.h"

#include "resources/material.h"

namespace ppp
{
    namespace material_pool
    {
        bool initialize();
        void terminate();

        bool has_material(const std::string& shader_tag);
        bool has_material(u64 id);

        resources::imaterial* material_at_shader_tag(const std::string& shader_tag);
        resources::imaterial* material_at_id(u64 id);

        resources::imaterial* material_instance_at_shader_tag(const std::string& shader_tag);
        resources::imaterial* material_instance_at_id(u64 id);

        void add_new_material(const resources::material& material);
        void set_active_material(const std::string& shader_tag);
        void reset_active_material();

        resources::imaterial* active_material();
        resources::imaterial* active_material_instance();
    }
}