#pragma once

#include "render/render_features.h"

#include "render/helpers/render_texture_registry.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace ppp
{
    namespace resources
    {
        using sampler_ids = std::vector<s32>;
        using texture_ids = std::vector<render::texture_id>;

        class material
        {
        public:
            material(const std::string& shader_tag);

            void ambient_color(const glm::vec4& ambient_color);
            void diffuse_color(const glm::vec4& diffuse_color);

            bool add_texture(render::texture_id id);
            void reset_textures();

            const u64 id() const;

            const glm::vec4& ambient_color() const;
            const glm::vec4& diffuse_color() const;
            
            const sampler_ids& samplers() const;
            const texture_ids& textures() const;

            const std::string& shader_tag() const;

        private:
            u64 m_id;

            const std::string m_shader_tag;

            glm::vec4 m_ambient_color;
            glm::vec4 m_diffuse_color;

            render::texture_registry m_texture_registry;
        };
    }
}

namespace std
{
    template<>
    struct std::hash<ppp::resources::material>
    {
        u64 operator()(const ppp::resources::material& m) const noexcept
        {
            return m.id();
        }
    };
}