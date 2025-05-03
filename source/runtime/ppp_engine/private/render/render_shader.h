#pragma once

#include "util/types.h"

#include "render/render_types.h"

#include <memory>
#include <string>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            class shader_program
            {
            public:
                shader_program(shading_model_type smtype, shading_blending_type btype, vertex_type vtype, std::string_view vs_source, std::string_view fs_source);
                shader_program(shading_model_type smtype, shading_blending_type btype, vertex_type vtype, std::string_view vs_source, std::string_view fs_source, std::string_view geometry_source);
                ~shader_program();

                void                    bind() const;
                void                    unbind() const;

                u32                     id() const;
                
                shading_model_type      shading_model() const;
                shading_blending_type   shading_blending() const;
                vertex_type             vertex_format() const;

            private:
                class impl;
                std::unique_ptr<impl>   m_pimpl;
            };
        }
    }
}

namespace std
{
    template<>
    struct hash<ppp::render::shaders::shader_program>
    {
        u64 operator()(const ppp::render::shaders::shader_program& s) const
        {
            return s.id();
        }
    };
}