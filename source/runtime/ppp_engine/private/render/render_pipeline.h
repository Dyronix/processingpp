#pragma once

#include "memory/memory_types.h"

namespace ppp
{
    namespace render
    {
        class render_pass;

        struct render_context;

        class render_pipeline 
        {
        public:
            void add_pass(graphics_unique_ptr<render_pass> pass);

            void execute(const render_context& context);

        private:
            graphics_vector<graphics_unique_ptr<render_pass>> m_passes;
        };

    }
}