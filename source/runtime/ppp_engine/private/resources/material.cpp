#include "resources/material.h"

namespace ppp
{
    namespace resources
    {
        //-------------------------------------------------------------------------
        // Material
        material::material(const std::string& shader_tag)
            : m_shader_tag(shader_tag)
            , m_ambient_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            , m_diffuse_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            , m_texture_registry(render::max_textures())
        {
            m_id = std::hash<std::string>{}(shader_tag);
        }

        //-------------------------------------------------------------------------
        void material::ambient_color(const glm::vec4& ambient_color) { m_ambient_color = ambient_color; }
        //-------------------------------------------------------------------------
        void material::diffuse_color(const glm::vec4& diffuse_color) { m_diffuse_color = diffuse_color; }

        //-------------------------------------------------------------------------
        s32 material::add_texture(render::texture_id id)
        {
            if (m_texture_registry.can_add(1))
            {
                return m_texture_registry.add_texture(id);
            }

            return false;
        }
        //-------------------------------------------------------------------------
        void material::reset_textures()
        {
            m_texture_registry.reset();
        }

        //-------------------------------------------------------------------------
        bool material::has_textures() const
        {
            return textures().size() > 0;
        }

        //-------------------------------------------------------------------------
        const u64 material::id() const
        {
            return m_id;
        }

        //-------------------------------------------------------------------------
        const glm::vec4 material::ambient_color() const { return m_ambient_color; }
        //-------------------------------------------------------------------------
        const glm::vec4 material::diffuse_color() const { return m_diffuse_color; }

        //-------------------------------------------------------------------------
        const sampler_ids& material::samplers() const { return m_texture_registry.samplers(); }
        //-------------------------------------------------------------------------
        const texture_ids& material::textures() const { return m_texture_registry.textures(); }

        //-------------------------------------------------------------------------
        const std::string& material::shader_tag() const { return m_shader_tag; }


        //-------------------------------------------------------------------------
        // Material Instance
        material_instance::material_instance(imaterial* base_material)
            : m_base_material(base_material)
        {}

        //-------------------------------------------------------------------------
        void material_instance::ambient_color(const glm::vec4& color) { m_ambient_color = color; }
        //-------------------------------------------------------------------------
        void material_instance::diffuse_color(const glm::vec4& color) { m_diffuse_color = color; }

        //-------------------------------------------------------------------------
        s32 material_instance::add_texture(render::texture_id id)
        {
            s32 sampler_id = m_base_material->add_texture(id);
            if (sampler_id >= 0)
            {
                m_samplers.push_back(sampler_id);
                return sampler_id;
            }

            return sampler_id;
        }
        //-------------------------------------------------------------------------
        void material_instance::reset_textures()
        {
            m_base_material->reset_textures();
        }

        //-------------------------------------------------------------------------
        bool material_instance::has_textures() const { return m_base_material->has_textures(); }

        //-------------------------------------------------------------------------
        const u64 material_instance::id() const { return m_base_material->id(); }

        //-------------------------------------------------------------------------
        const glm::vec4 material_instance::ambient_color() const { return m_ambient_color.value_or(m_base_material->ambient_color()); }
        //-------------------------------------------------------------------------
        const glm::vec4 material_instance::diffuse_color() const { return m_diffuse_color.value_or(m_base_material->diffuse_color()); }

        //-------------------------------------------------------------------------
        const sampler_ids& material_instance::samplers() const { return m_samplers; }
        //-------------------------------------------------------------------------
        const texture_ids& material_instance::textures() const { return m_base_material->textures(); }

        //-------------------------------------------------------------------------
        const std::string& material_instance::shader_tag() const { return m_base_material->shader_tag(); }
    }
}