#pragma once

#include "util/types.h"

#include "string/string_id.h"

#include <memory>
#include <vector>
#include <unordered_map>

namespace ppp
{
    namespace render
    {
        class irender_pass;

        struct render_context;

        enum class insertion_point
        {
            BEFORE_UNLIT_OPAQUE,
            AFTER_UNLIT_OPAQUE,
            BEFORE_LIT_OPAQUE,
            AFTER_LIT_OPAQUE
        };

        class render_pipeline 
        {
        public:
            ~render_pipeline();

        public:
            bool has_pass_with_framebuffer_tag(string::string_id shader_tag) const;
            bool has_pass_with_shader_tag(string::string_id shader_tag) const;

        public:
            void add_insertion_point(insertion_point point);

            irender_pass* add_pass(std::unique_ptr<irender_pass> pass);
            irender_pass* insert_pass(insertion_point point, std::unique_ptr<irender_pass> pass);

        public:
            void execute(const render_context& context);

        private:
            std::vector<std::unique_ptr<irender_pass>> m_passes;
            std::unordered_map<insertion_point, s32> m_insertion_points;
        };

    }
}