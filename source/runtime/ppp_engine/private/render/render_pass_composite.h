#pragma once

#include "render/render_pass.h"

#include "util/types.h"

#include <type_traits>
#include <memory>
#include <vector>

namespace ppp
{
    namespace render
    {
        class base_composite_pass : public irender_pass
        {
        public:
            //-------------------------------------------------------------------------
            base_composite_pass(string::string_id pass_tag)
                :irender_pass(pass_tag)
            {}

        public:
            virtual const std::vector<std::unique_ptr<geometry_render_pass>>& children() const = 0;
        };

        template<typename T>
        class composite_pass : public base_composite_pass
        {
            static_assert(std::is_base_of_v<geometry_render_pass, T>, "T must derive from geometry_render_pass");

        public:
            //-------------------------------------------------------------------------
            composite_pass(string::string_id pass_tag)
                :base_composite_pass(pass_tag)
            {
                
            }

        public:
            const std::vector<std::unique_ptr<geometry_render_pass>>& children() const override
            {
                return m_children;
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
            bool has_shader() const override
            {
                for (auto& child : m_children)
                {
                    if (child->has_shader())
                    {
                        return true;
                    }
                }

                return false;
            }

            //-------------------------------------------------------------------------
            s32 count_batched_draw_calls(const render_context& context) const override
            {
                s32 total = 0;
                for (auto& child : children())
                {
                    total += child->count_batched_draw_calls(context);
                }
                return total;
            }
            //-------------------------------------------------------------------------
            s32 count_instanced_draw_calls(const render_context& context) const override
            {
                s32 total = 0;
                for (auto& child : children()) 
                {
                    total += child->count_instanced_draw_calls(context);
                }
                return total;
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
            std::vector<std::unique_ptr<geometry_render_pass>> m_children;
        };

    }
}
