#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

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
                shader_program(std::string_view vs_source, std::string_view fs_source);
                shader_program(std::string_view vs_source, std::string_view fs_source, std::string_view geometry_source);
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