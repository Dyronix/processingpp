#include "resources/material.h"

namespace ppp
{
    namespace resources
    {
        //-------------------------------------------------------------------------
        material::material(const std::string& shader_tag)
            : m_shader_tag(shader_tag)
            , m_ambient_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            , m_diffuse_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
            , m_texture_registry(render::max_textures())
        {
            m_id = std::hash<std::string>{}(shader_tag);
        }

        //-------------------------------------------------------------------------
        const u64 material::id() const
        {
            return m_id;
        }

        //-------------------------------------------------------------------------
        void material::ambient_color(const glm::vec4& ambient_color) { m_ambient_color = ambient_color; }
        //-------------------------------------------------------------------------
        void material::diffuse_color(const glm::vec4& diffuse_color) { m_diffuse_color = diffuse_color; }

        //-------------------------------------------------------------------------
        bool material::add_texture(render::texture_id id)
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
        const glm::vec4& material::ambient_color() const { return m_ambient_color; }
        //-------------------------------------------------------------------------
        const glm::vec4& material::diffuse_color() const { return m_diffuse_color; }

        //-------------------------------------------------------------------------
        const sampler_ids& material::samplers() const { return m_texture_registry.samplers(); }
        //-------------------------------------------------------------------------
        const texture_ids& material::textures() const { return m_texture_registry.textures(); }

        //-------------------------------------------------------------------------
        const std::string& material::shader_tag() const { return m_shader_tag; }
    }
}