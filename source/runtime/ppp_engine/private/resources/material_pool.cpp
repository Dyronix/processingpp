#include "resources/material_pool.h"

namespace ppp
{
    namespace material_pool
    {
        namespace internal
        {
            struct material_id_hasher
            {
                u64 operator()(u64 key) const
                {
                    // No hashing required for the material id as it is already hashed
                    return key;
                }
            };

            std::unordered_map<u64, resources::material, material_id_hasher> _materials;

            resources::material* _active_material = nullptr;
        }

        //-------------------------------------------------------------------------
        bool initialize()
        {
            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            internal::_materials.clear();
        }

        //-------------------------------------------------------------------------
        bool has_material(const std::string& shader_tag)
        {
            return has_material(std::hash<std::string>{}(shader_tag));
        }

        //-------------------------------------------------------------------------
        bool has_material(u64 id)
        {
            return internal::_materials.find(id) != std::cend(internal::_materials);
        }

        //-------------------------------------------------------------------------
        resources::material* material_at_shader_tag(const std::string& shader_tag)
        {
            return material_at_id(std::hash<std::string>{}(shader_tag));
        }

        //-------------------------------------------------------------------------
        resources::material* material_at_id(u64 id)
        {
            return &internal::_materials.at(id);
        }

        //-------------------------------------------------------------------------
        void add_new_material(const resources::material& material)
        {
            internal::_materials.insert(std::make_pair(material.id(), material));
        }

        //-------------------------------------------------------------------------
        void set_active_material(const std::string& shader_tag)
        {
            internal::_active_material = material_at_shader_tag(shader_tag))
        }

        //-------------------------------------------------------------------------
        void reset_active_material()
        {
            internal::_active_material = nullptr;
        }

        //-------------------------------------------------------------------------
        resources::material* active_material()
        {
            return internal::_active_material;
        }
    }
}