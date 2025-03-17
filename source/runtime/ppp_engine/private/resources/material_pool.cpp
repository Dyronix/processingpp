#include "resources/material_pool.h"
#include "resources/shader_pool.h"
#include "util/log.h"


#include "string/string_id.h"

namespace ppp
{
    namespace material_pool
    {
        using materials_hash_map            = std::unordered_map<string::string_id, resources::material>;
        using material_instances_hash_map   = std::unordered_map<u64, resources::material_instance>;
        using registred_images_hash_map     = std::unordered_map<string::string_id, std::vector<render::texture_id>>;

        struct context
        {
            materials_hash_map          materials_hash_map;
            material_instances_hash_map material_instances_hash_map;

            registred_images_hash_map   registered_images;
        } g_ctx;

        namespace texture_cache
        {
            //-------------------------------------------------------------------------
            void add_image(string::string_id shader_tag, render::texture_id image)
            {
                auto it = g_ctx.registered_images.find(shader_tag);
                if (it == std::cend(g_ctx.registered_images))
                {
                    g_ctx.registered_images[shader_tag] = { image };
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

            //-------------------------------------------------------------------------
            void reset_images(string::string_id shader_tag)
            {
                auto it = g_ctx.registered_images.find(shader_tag);
                if (it != std::cend(g_ctx.registered_images))
                {
                    g_ctx.registered_images.at(shader_tag).clear();
                }
            }

            //-------------------------------------------------------------------------
            const std::vector<render::texture_id>& images(string::string_id shader_tag)
            {
                auto it = g_ctx.registered_images.find(shader_tag);
                if (it != std::cend(g_ctx.registered_images))
                {
                    return it->second;
                }

                static std::vector<render::texture_id> empty_ids;
                return empty_ids;
            }
        }

        //-------------------------------------------------------------------------
        bool initialize()
        {
            // Add all materials
            // 
            // batched
            // unlit
            add_new_material(resources::material(shader_pool::tags::unlit::color()));
            add_new_material(resources::material(shader_pool::tags::unlit::texture()));
            add_new_material(resources::material(shader_pool::tags::unlit::font())); 
            add_new_material(resources::material(shader_pool::tags::unlit::normal()));         
            add_new_material(resources::material(shader_pool::tags::unlit::shadow()));         
            // lit
            add_new_material(resources::material(shader_pool::tags::lit::color()));
            add_new_material(resources::material(shader_pool::tags::lit::specular()));
            // instanced
            // unlit
            add_new_material(resources::material(shader_pool::tags::unlit::instance_normal()));
            add_new_material(resources::material(shader_pool::tags::unlit::instance_color()));
            add_new_material(resources::material(shader_pool::tags::unlit::instance_texture()));
            add_new_material(resources::material(shader_pool::tags::unlit::instance_shadow()));
            // lit
            add_new_material(resources::material(shader_pool::tags::lit::instance_color()));
            add_new_material(resources::material(shader_pool::tags::lit::instance_specular()));

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            g_ctx.materials_hash_map.clear();
            g_ctx.material_instances_hash_map.clear();
        }

        //-------------------------------------------------------------------------
        bool has_material(string::string_id shader_tag)
        {
            return g_ctx.materials_hash_map.find(shader_tag) != std::cend(g_ctx.materials_hash_map);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_at_shader_tag(string::string_id shader_tag)
        {
            return &g_ctx.materials_hash_map.at(shader_tag);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* material_instance_at_shader_tag(string::string_id shader_tag)
        {
            auto it = std::find_if(std::begin(g_ctx.material_instances_hash_map), std::end(g_ctx.material_instances_hash_map),
                [shader_tag](const auto& pair)
                {
                    return pair.second.shader_tag() == shader_tag;
                });

            if (it != std::cend(g_ctx.material_instances_hash_map))
            {
                return &it->second;
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        void add_new_material(const resources::material& material)
        {
            g_ctx.materials_hash_map.emplace(material.shader_tag(), material);
        }

        //-------------------------------------------------------------------------
        resources::imaterial* get_or_create_material_instance(string::string_id shader_tag)
        {
            auto mat = material_at_shader_tag(shader_tag);
            auto cache = g_ctx.registered_images.find(shader_tag);

            if (mat == nullptr)
            {
                log::error("No material found for given tag: {}", string::restore_sid(shader_tag));
                return nullptr;
            }

            // Hash active textures for uniqueness
            u64 hash = mat->shader_tag().value();

            if (cache != std::cend(g_ctx.registered_images))
            {
                for (auto texture : g_ctx.registered_images.at(shader_tag))
                {
                    hash ^= std::hash<u32>{}(texture)+0x9e3779b9 + (hash << 6) + (hash >> 2);
                }
            }

            // Check if instance already exists
            auto it = g_ctx.material_instances_hash_map.find(hash);
            if (it != g_ctx.material_instances_hash_map.end()) 
            {
                return &it->second;
            }

            // Create a new material instance
            resources::material_instance instance(mat, g_ctx.registered_images.size());

            log::info("Material instance ({0}) added for shader with tag: {1}", hash, string::restore_sid(shader_tag));

            // Assign textures
            if (cache != std::cend(g_ctx.registered_images))
            {
                for (auto texture : g_ctx.registered_images.at(shader_tag))
                {
                    instance.add_texture(texture);
                }
            }

            // Cache the instance
            g_ctx.material_instances_hash_map.emplace(hash, std::move(instance));

            return &g_ctx.material_instances_hash_map.at(hash);
        }
    }
}