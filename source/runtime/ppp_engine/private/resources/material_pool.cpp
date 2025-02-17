#include "resources/material_pool.h"
#include "resources/shader_pool.h"
#include "util/log.h"
#include "memory/memory_tracker.h"
#include "string/string_id.h"

namespace ppp
{
    namespace material_pool
    {
        namespace internal
        {
            using materials_hash_map = graphics_hash_map<string::string_id, resources::material>;
            using material_instances_hash_map = graphics_hash_map<u64, resources::material_instance>;
            using registred_images_hash_map = graphics_hash_map<string::string_id, graphics_vector<render::texture_id>>;

            static materials_hash_map& materials()
            {
                static materials_hash_map* s_material = memory::create_tagged_new<materials_hash_map, memory::persistent_tagged_policy, memory::tags::graphics>();
                return *s_material;
            }
            static material_instances_hash_map& material_instances()
            {
                static material_instances_hash_map* s_material_instances = memory::create_tagged_new<material_instances_hash_map, memory::persistent_tagged_policy, memory::tags::graphics>();
                return *s_material_instances;
            }

            static registred_images_hash_map& registered_images()
            {
                static registred_images_hash_map* s_registered_images = memory::create_tagged_new<registred_images_hash_map, memory::persistent_tagged_policy, memory::tags::graphics>();
                return *s_registered_images;
            }
        }

        namespace texture_cache
        {
            void add_image(string::string_id shader_tag, render::texture_id image)
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

            void reset_images(string::string_id shader_tag)
            {
                auto it = internal::registered_images().find(shader_tag);
                if (it != std::cend(internal::registered_images()))
                {
                    internal::registered_images().at(shader_tag).clear();
                }
            }

            const graphics_vector<render::texture_id>& images(string::string_id shader_tag)
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
        bool has_material(string::string_id shader_tag)
        {
            return internal::materials().find(shader_tag) != std::cend(internal::materials());
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_at_shader_tag(string::string_id shader_tag)
        {
            return &internal::materials().at(shader_tag);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_instance_at_shader_tag(string::string_id shader_tag)
        {
            auto it = std::find_if(std::begin(internal::material_instances()), std::end(internal::material_instances()),
                [shader_tag](const auto& pair)
                {
                    return pair.second.shader_tag() == shader_tag;
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
            internal::materials().emplace(material.shader_tag(), material);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* get_or_create_material_instance(string::string_id shader_tag)
        {
            auto mat = material_at_shader_tag(shader_tag);
            auto cache = internal::registered_images().find(shader_tag);

            if (mat == nullptr)
            {
                log::error("No material found for given tag: {}", string::restore_sid(shader_tag));
                return nullptr;
            }

            // Hash active textures for uniqueness
            u64 hash = mat->shader_tag().value();

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

            log::info("material instance ({0}) added for shader tag: {1}", hash, string::restore_sid(shader_tag));

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