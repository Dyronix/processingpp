#include "resources/material_pool.h"
#include "resources/shader_pool.h"

#include "util/log.h"

#include "memory/memory_tracker.h"

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

            static graphics_hash_map<u64, resources::material, material_id_hasher>& materials()
            {
                static graphics_hash_map<u64, resources::material, material_id_hasher> s_material;

                return s_material;
            }
            static graphics_hash_map<u64, resources::material_instance>& material_instances()
            {
                static graphics_hash_map<u64, resources::material_instance> s_material_instances;

                return s_material_instances;
            }

            static graphics_hash_map<std::string_view, graphics_vector<render::texture_id>>& registered_images()
            {
                static graphics_hash_map<std::string_view, graphics_vector<render::texture_id>> s_registered_images;

                return s_registered_images;
            }
        }

        namespace texture_cache
        {
            void add_image(std::string_view shader_tag, render::texture_id image)
            {
                auto it = internal::registered_images().find(shader_tag);
                if (it == std::cend(internal::registered_images()))
                {
                    internal::registered_images()[shader_tag] = { image };
                }
                else
                {
                    auto image_it = std::find(std::cbegin(it->second), std::cend(it->second), image);
                    if (image_it == std::cend(it->second))
                    {
                        it->second.push_back(image);
                    }
                }
            }

            void reset_images(std::string_view shader_tag)
            {
                auto it = internal::registered_images().find(shader_tag);
                if (it != std::cend(internal::registered_images()))
                {
                    internal::registered_images().at(shader_tag).clear();
                }
            }

            const graphics_vector<render::texture_id>& images(std::string_view shader_tag)
            {
                auto it = internal::registered_images().find(shader_tag);
                if (it != std::cend(internal::registered_images()))
                {
                    return it->second;
                }

                static graphics_vector<render::texture_id> empty_ids;
                return empty_ids;
            }
        }

        //-------------------------------------------------------------------------
        bool initialize()
        {
            // Add all materials
            add_new_material(resources::material(shader_pool::tags::unlit_color()));
            add_new_material(resources::material(shader_pool::tags::instance_unlit_color()));
            
            add_new_material(resources::material(shader_pool::tags::unlit_texture()));
            add_new_material(resources::material(shader_pool::tags::instance_unlit_texture()));
            
            add_new_material(resources::material(shader_pool::tags::unlit_font()));
            
            add_new_material(resources::material(shader_pool::tags::unlit_normal()));
            add_new_material(resources::material(shader_pool::tags::instance_unlit_normal()));
            
            add_new_material(resources::material(shader_pool::tags::lit_specular()));
            add_new_material(resources::material(shader_pool::tags::instance_lit_specular()));

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            internal::materials().clear();
            internal::material_instances().clear();
        }

        //-------------------------------------------------------------------------
        bool has_material(std::string_view shader_tag)
        {
            return has_material(std::hash<std::string_view>{}(shader_tag));
        }

        //-------------------------------------------------------------------------
        bool has_material(u64 id)
        {
            return internal::materials().find(id) != std::cend(internal::materials());
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_at_shader_tag(std::string_view shader_tag)
        {
            return material_at_id(std::hash<std::string_view>{}(shader_tag));
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_at_id(u64 id)
        {
            return &internal::materials().at(id);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_instance_at_shader_tag(std::string_view shader_tag)
        {
            return material_instance_at_id(std::hash<std::string_view>{}(shader_tag));
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_instance_at_id(u64 id)
        {
            auto it = std::find_if(std::begin(internal::material_instances()), std::end(internal::material_instances()),
                [id](const auto& pair)
            {
                return pair.second.id() == id;
            });

            if (it != std::cend(internal::material_instances()))
            {
                return &it->second;
            }
                
            return nullptr;
        }

        //-------------------------------------------------------------------------
        void add_new_material(const resources::material& material)
        {
            internal::materials().emplace(material.id(), material);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* get_or_create_material_instance(std::string_view shader_tag)
        {
            auto mat = material_at_shader_tag(shader_tag);
            auto cache = internal::registered_images().find(shader_tag);

            if (mat == nullptr)
            {
                log::error("No material found for given tag: {}", shader_tag);
                return nullptr;
            }

            // Hash active textures for uniqueness
            size_t hash = mat->id();

            if (cache != std::cend(internal::registered_images()))
            {
                for (auto texture : internal::registered_images().at(shader_tag))
                {
                    hash ^= std::hash<u32>{}(texture)+0x9e3779b9 + (hash << 6) + (hash >> 2);
                }
            }

            // Check if instance already exists
            auto it = internal::material_instances().find(hash);
            if (it != internal::material_instances().end()) 
            {
                return &it->second;
            }

            // Create a new material instance
            resources::material_instance instance(mat, internal::registered_images().size());

            log::info("material instance ({0}) added for shader tag: {1}", hash, shader_tag);

            // Assign textures
            if (cache != std::cend(internal::registered_images()))
            {
                for (auto texture : internal::registered_images().at(shader_tag))
                {
                    instance.add_texture(texture);
                }
            }

            // Cache the instance
            internal::material_instances().emplace(hash, std::move(instance));

            return &internal::material_instances().at(hash);
        }
    }
}