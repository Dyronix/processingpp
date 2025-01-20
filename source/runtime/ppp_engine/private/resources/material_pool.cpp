#include "resources/material_pool.h"
#include "resources/shader_pool.h"
#include "resources/texture_manager.h"

#include "util/log.h"

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
            std::unordered_map<u64, resources::material_instance> _material_instances;

            resources::imaterial* _active_material = nullptr;
        }

        //-------------------------------------------------------------------------
        bool initialize()
        {
            // Add all materials
            add_new_material(resources::material(shader_pool::tags::unlit_color));
            add_new_material(resources::material(shader_pool::tags::instance_unlit_color));
            
            add_new_material(resources::material(shader_pool::tags::unlit_texture));
            add_new_material(resources::material(shader_pool::tags::instance_unlit_texture));
            
            add_new_material(resources::material(shader_pool::tags::unlit_font));
            
            add_new_material(resources::material(shader_pool::tags::unlit_normal));
            add_new_material(resources::material(shader_pool::tags::instance_unlit_normal));
            
            add_new_material(resources::material(shader_pool::tags::lit_specular));
            add_new_material(resources::material(shader_pool::tags::instance_lit_specular));

            // Set one active
            set_active_material(shader_pool::tags::unlit_texture);

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
        resources::imaterial* material_at_shader_tag(const std::string& shader_tag)
        {
            return material_at_id(std::hash<std::string>{}(shader_tag));
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_at_id(u64 id)
        {
            return &internal::_materials.at(id);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_instance_at_shader_tag(const std::string& shader_tag)
        {
            return material_instance_at_id(std::hash<std::string>{}(shader_tag));
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_instance_at_id(u64 id)
        {
            auto it = std::find_if(std::begin(internal::_material_instances), std::end(internal::_material_instances),
                [id](const auto& pair)
            {
                return pair.second.id() == id;
            });

            if (it != std::cend(internal::_material_instances))
            {
                return &it->second;
            }
                
            return nullptr;
        }

        //-------------------------------------------------------------------------
        void add_new_material(const resources::material& material)
        {
            internal::_materials.insert(std::make_pair(material.id(), material));
        }

        //-------------------------------------------------------------------------
        void set_active_material(const std::string& shader_tag)
        {
            internal::_active_material = material_at_shader_tag(shader_tag);
        }

        //-------------------------------------------------------------------------
        void reset_active_material()
        {
            internal::_active_material = nullptr;
        }

        //-------------------------------------------------------------------------
        resources::imaterial* active_material()
        {
            return internal::_active_material;
        }

        //-------------------------------------------------------------------------
        resources::imaterial* active_material_instance()
        {
            assert(active_material());

            // Hash active textures for uniqueness
            size_t texture_hash = 0;
            for (auto texture : texture_manager::images(active_material()->shader_tag())) 
            {
                texture_hash ^= std::hash<u32>{}(texture)+0x9e3779b9 + (texture_hash << 6) + (texture_hash >> 2);
            }

            // Check if instance already exists
            auto it = internal::_material_instances.find(texture_hash);
            if (it != internal::_material_instances.end()) 
            {
                return &it->second;
            }

            // Create a new material instance
            resources::material_instance instance(static_cast<resources::material*>(active_material()));

            log::info("material instance added: {}", texture_hash);

            // Assign textures
            for (auto texture : texture_manager::images(active_material()->shader_tag())) 
            {
                instance.add_texture(texture);
            }

            // Cache the instance
            internal::_material_instances.emplace(texture_hash, std::move(instance));

            return &internal::_material_instances.at(texture_hash);
        }
    }
}