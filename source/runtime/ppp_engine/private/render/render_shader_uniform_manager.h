#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include "string/string_id.h"

#include <glm/glm.hpp>

#include <string>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, bool value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, s32 value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, f32 value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::vec2& value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::vec3& value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::vec4& value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::mat2& value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::mat3& value);
            void push_uniform(u32 shader_program_id, string::string_id uniform_name, const glm::mat4& value);

            void push_uniform_array(u32 shader_program_id, string::string_id uniform_name, u64 count, const s32* value);
            void push_uniform_array(u32 shader_program_id, string::string_id uniform_name, u64 count, const u32* value);
            void push_uniform_array(u32 shader_program_id, string::string_id uniform_name, u64 count, const f32* value);

            void apply_uniforms(u32 shader_program_id);
        }
    }
}