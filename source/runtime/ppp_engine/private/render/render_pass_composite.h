#pragma once

#include "render/render_pass.h"

#include <type_traits>
#include <memory>
#include <vector>

namespace ppp
{
    namespace render
    {
        template<typename T>
        class composite_pass : public irender_pass
        {
            static_assert(std::is_base_of_v<geometry_render_pass, T>, "T must derive from geometry_render_pass");

        public:
            //-------------------------------------------------------------------------
            composite_pass(string::string_id pass_tag)
                :irender_pass(pass_tag)
            {
                
            }

        public:
            //-------------------------------------------------------------------------
            bool has_framebuffer_tag(string::string_id framebuffer_tag) const override
            {
                for (auto& child : m_children)
                {
                    if (child->has_framebuffer_tag(framebuffer_tag))
                    {
                        return true;
                    }
                }

                return false;
            }

            //-------------------------------------------------------------------------
            bool has_shader(string::string_id shader_tag) const override
            {
                for (auto& child : m_children)
                {
                    if (child->has_shader(shader_tag))
                    {
                        return true;
                    }
                }

                return false;
            }

            //-------------------------------------------------------------------------
            void add_child(std::unique_ptr<T> pass)
            {
                m_children.push_back(std::move(pass));
            }

            //-------------------------------------------------------------------------
            void begin_frame(const render_context& context) override
            {
                // Nothing to implement
            }

            //-------------------------------------------------------------------------
            void render(const render_context& context) override
            {
                for (auto& child : m_children)
                {
                    if (child->should_render(context))
                    {
                        child->begin_frame(context);
                        child->render(context);
                        child->end_frame(context);
                    }
                }
            }

            //-------------------------------------------------------------------------
            void end_frame(const render_context& context) override
            {
                // Nothing to implement
            }

        private:
            std::vector<std::unique_ptr<T>> m_children;
        };

    }
}
