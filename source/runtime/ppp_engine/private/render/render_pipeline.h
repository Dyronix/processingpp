#pragma once

#include <memory>
#include <vector>

namespace ppp
{
    namespace render
    {
        class render_pass;

        struct render_context;

        class render_pipeline 
        {
        public:
            ~render_pipeline();

            void add_pass(std::unique_ptr<render_pass> pass);

            void execute(const render_context& context);

        private:
            std::vector<std::unique_ptr<render_pass>> m_passes;
        };

    }
}