#pragma once

#include "render/render_item.h"
#include "render/render_types.h"

#include "util/types.h"

#include <string>
#include <functional>

namespace ppp
{
    namespace resources
    {
        class material;
    }

    namespace render
    {
        class base_renderer
        {
        public:
            base_renderer(const attribute_layout* layouts, u64 layout_count, const std::string& shader_tag);
            virtual ~base_renderer();

        public:
            virtual void                begin() = 0;
            virtual void                render(const glm::mat4& vp) = 0;
            virtual void                end() = 0;
            virtual void                terminate() = 0;

        public:
            void                        enable_solid_rendering(bool enable);
            void                        enable_wireframe_rendering(bool enable);

            void                        user_shader_program(const std::string& tag);
            void                        reset_user_shader_program();

        public:
            bool                        solid_rendering_supported() const;
            bool                        wireframe_rendering_supported() const;

            u32                         shader_program() const;

            resources::material*        material() const;


        protected:
            s32                         rasterization_mode() const { return m_rasterization_mode; }

            const attribute_layout*     layouts() const { return m_layouts; }
            u64                         layout_count() const { return m_layout_count; }

        private:
            std::string                 m_user_shader_tag;
            std::string                 m_shader_tag;

            s32                         m_rasterization_mode;

            const attribute_layout*     m_layouts;
            u64                         m_layout_count;
        };
    }
}