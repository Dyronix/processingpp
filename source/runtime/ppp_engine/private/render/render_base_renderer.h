#pragma once

#include "render/render_item.h"
#include "render/render_shader.h"

#include "util/types.h"

#include "memory/memory_types.h"

#include "string/string_id.h"

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
            virtual void                render() = 0;
            virtual void                end() = 0;
            virtual void                terminate() = 0;

            

        public:
            void                        enable_solid_rendering(bool enable);
            void                        enable_wireframe_rendering(bool enable);

        public:
            bool                        has_texture_support() const;

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
            global_unique_ptr<impl>       m_pimpl;
        };
    }
}