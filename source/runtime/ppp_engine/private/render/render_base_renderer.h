#pragma once

#include "render/render_item.h"
#include "render/render_types.h"
#include "render/render_shader.h"

#include "util/types.h"



#include "string/string_id.h"

#include <string>
#include <functional>

namespace ppp
{
    namespace resources
    {
        class imaterial;

        using shader_program = std::shared_ptr<render::shaders::shader_program>;
    }

    namespace render
    {
        class base_renderer
        {
        public:
            base_renderer(string::string_id shader_tag);
            virtual ~base_renderer();

        public:
            virtual void                begin() = 0;
            virtual void                render(const glm::vec3& camera_position, const glm::vec3& camera_target, const glm::mat4& lightvp, const glm::mat4& vp) = 0;
            virtual void                end() = 0;
            virtual void                terminate() = 0;

        public:
            void                        enable_solid_rendering(bool enable);
            void                        enable_wireframe_rendering(bool enable);

            void                        user_shader_program(string::string_id tag);
            void                        reset_user_shader_program();

        public:
            bool                        solid_rendering_supported() const;
            bool                        wireframe_rendering_supported() const;

            resources::shader_program   shader_program() const;
            resources::imaterial*       material() const;


        protected:
            s32                         rasterization_mode() const;

            const attribute_layout*     layouts() const;
            u64                         layout_count() const;

        private:
            class impl;
            std::unique_ptr<impl>     m_pimpl;
        };
    }
}