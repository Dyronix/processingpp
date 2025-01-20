#pragma once

#include "render/render_features.h"

#include "render/helpers/render_texture_registry.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <optional>

namespace ppp
{
    namespace resources
    {
        using sampler_ids = std::vector<s32>;
        using texture_ids = std::vector<render::texture_id>;

        class imaterial
        {
        public:
            virtual bool has_textures() const = 0;

            virtual void ambient_color(const glm::vec4& ambient_color) = 0;
            virtual void diffuse_color(const glm::vec4& diffuse_color) = 0;

            virtual s32 add_texture(render::texture_id id) = 0;
            virtual void reset_textures() = 0;

            virtual const u64 id() const = 0;

            virtual const glm::vec4 ambient_color() const = 0;
            virtual const glm::vec4 diffuse_color() const = 0;

            virtual const sampler_ids& samplers() const = 0;
            virtual const texture_ids& textures() const = 0;

            virtual const std::string& shader_tag() const = 0;
        };

        class material : public imaterial
        {
        public:
            material(const std::string& shader_tag);

        public:
            void ambient_color(const glm::vec4& ambient_color) override;
            void diffuse_color(const glm::vec4& diffuse_color) override;

        public:
            s32 add_texture(render::texture_id id) override;

            void reset_textures() override;

        public:
            bool has_textures() const override;

            const u64 id() const override;

            const glm::vec4 ambient_color() const override;
            const glm::vec4 diffuse_color() const override;
            
            const sampler_ids& samplers() const override;
            const texture_ids& textures() const override;

            const std::string& shader_tag() const override;

        private:
            u64 m_id;

            const std::string m_shader_tag;

            glm::vec4 m_ambient_color;
            glm::vec4 m_diffuse_color;

            render::texture_registry m_texture_registry;
        };

        class material_instance : public imaterial
        {
        public:
            material_instance(material* base_material);

        public:
            void ambient_color(const glm::vec4& color);
            void diffuse_color(const glm::vec4& color);

        public:
            s32 add_texture(render::texture_id id);

            void reset_textures() override;

        public:
            bool has_textures() const override;

            const u64 id() const override;

            const glm::vec4 ambient_color() const override;
            const glm::vec4 diffuse_color() const override;

            const sampler_ids& samplers() const override;
            const texture_ids& textures() const override;

            const std::string& shader_tag() const override;

        private:
            material* m_base_material;

            std::optional<glm::vec4> m_ambient_color;
            std::optional<glm::vec4> m_diffuse_color;

            sampler_ids m_samplers;
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