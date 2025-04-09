#pragma once

#include "util/types.h"

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
            BEFORE_OPAQUE,
            AFTER_OPAQUE
        };

        class render_pipeline 
        {
        public:
            ~render_pipeline();

            void add_pass(std::unique_ptr<irender_pass> pass);
            void insert_pass(insertion_point point, std::unique_ptr<irender_pass> pass);

            void add_insertion_point(insertion_point point);

            void execute(const render_context& context);

        private:
            std::vector<std::unique_ptr<irender_pass>> m_passes;
            std::unordered_map<insertion_point, s32> m_insertion_points;
        };

    }
}