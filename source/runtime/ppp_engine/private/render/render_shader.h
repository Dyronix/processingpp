#pragma once

#include "util/types.h"

#include <memory>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            class shader_program
            {
            public:
                shader_program(const std::string& vs_source, const std::string& fs_source);
                shader_program(const std::string& vs_source, const std::string& fs_source, const std::string& geometry_source);
                ~shader_program();

                void bind() const;
                void unbind() const;

                u32 id() const;

            private:
                u32 m_program_id = 0;
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