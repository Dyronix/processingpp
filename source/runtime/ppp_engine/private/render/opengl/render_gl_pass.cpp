#include "render/render_context.h"
#include "render/render_pass.h"

#include "render/render_batch_data_table.h"
#include "render/render_instance_data_table.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "resources/framebuffer_pool.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        framebuffer_render_pass::framebuffer_render_pass(string::string_id pass_tag, string::string_id framebuffer_tag, s32 framebuffer_flags)
            : irender_pass(pass_tag)
            , m_framebuffer_tag(framebuffer_tag)
            , m_framebuffer_flags(framebuffer_flags)
        {}

        //-------------------------------------------------------------------------
        framebuffer_render_pass::~framebuffer_render_pass() = default;

        //-------------------------------------------------------------------------
        bool framebuffer_render_pass::has_framebuffer_tag(string::string_id framebuffer_tag) const
        {
            if (m_framebuffer_tag.is_none())
            {
                return false;
            }

            return framebuffer_tag == m_framebuffer_tag;
        }

        //-------------------------------------------------------------------------
        const resources::iframebuffer* framebuffer_render_pass::framebuffer() const
        {
            auto framebuffer = framebuffer_pool::get(m_framebuffer_tag, m_framebuffer_flags);

            return framebuffer;
        }

        //-------------------------------------------------------------------------
        geometry_render_pass::geometry_render_pass(string::string_id pass_tag, string::string_id shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags, draw_mode draw_mode)
            : framebuffer_render_pass(pass_tag, framebuffer_tag, framebuffer_flags)
            , m_shader_tag(shader_tag)
            , m_draw_mode(draw_mode)
        {}

        //-------------------------------------------------------------------------
        geometry_render_pass::~geometry_render_pass() = default;

        //-------------------------------------------------------------------------
        bool geometry_render_pass::should_render(const render_context& context) const
        {
            bool can_draw = false;

            if (m_draw_mode == draw_mode::BATCHED || m_draw_mode == draw_mode::AUTO)
            {
                can_draw |= context.batch_data->empty() == false;
            }

            if (m_draw_mode == draw_mode::INSTANCED || m_draw_mode == draw_mode::AUTO)
            {
                can_draw |= context.instance_data->empty() == false;
            }

            return can_draw;
        }

        //-------------------------------------------------------------------------
        bool geometry_render_pass::has_shader(string::string_id shader_tag) const
        {
            if (m_shader_tag.is_none())
            {
                return false;
            }

            return shader_tag == m_shader_tag;
        }

        //-------------------------------------------------------------------------
        bool geometry_render_pass::has_shader() const
        {
            return !m_shader_tag.is_none();
        }

        //-------------------------------------------------------------------------
        bool geometry_render_pass::batch_rendering_enabled() const
        {
            return m_draw_mode == draw_mode::BATCHED || m_draw_mode == draw_mode::AUTO;
        }
        
        //-------------------------------------------------------------------------
        bool geometry_render_pass::instance_rendering_enabled() const
        {
            return m_draw_mode == draw_mode::INSTANCED || m_draw_mode == draw_mode::AUTO;
        }

        //-------------------------------------------------------------------------
        s32 geometry_render_pass::count_batched_draw_calls(const render_context& context) const
        {
            if (!batch_rendering_enabled() || !has_shader())
            {
                return 0;
            }

            s32 draw_calls = 0;
            for (auto& pair : *context.batch_data)
            {
                const auto& key = pair.first;
                if (key.shader_tag == shader_tag())
                {
                    draw_calls += pair.second->size();
                }
            }

            return draw_calls;
        }

        //-------------------------------------------------------------------------
        s32 geometry_render_pass::count_instanced_draw_calls(const render_context& context) const
        {
            if (!instance_rendering_enabled() || !has_shader())
            {
                return 0;
            }

            s32 draw_calls = 0;
            for (auto& pair : *context.instance_data)
            {
                const auto& key = pair.first;
                if (key.shader_tag == shader_tag())
                {
                    draw_calls += pair.second->size();
                }
            }

            return draw_calls;
        }

        //-------------------------------------------------------------------------
        const resources::shader_program geometry_render_pass::shader_program() const
        {
            auto m = material();
            auto p = shader_pool::get_shader_program(m->shader_tag());

            return p;
        }

        //-------------------------------------------------------------------------
        const resources::imaterial* geometry_render_pass::material() const
        {
            return material_pool::material_at_shader_tag(m_shader_tag);
        }

        //-------------------------------------------------------------------------
        ibatch_render_strategy* geometry_render_pass::batch_render_strategy()
        {
            assert(batch_rendering_enabled());

            if (m_batch_draw_strategy == nullptr)
            {
                m_batch_draw_strategy = make_batch_render_strategy();
            }

            return m_batch_draw_strategy.get();
        }

        //-------------------------------------------------------------------------
        iinstance_render_strategy* geometry_render_pass::instance_render_strategy()
        {
            assert(instance_rendering_enabled());

            if (m_inst_draw_strategy == nullptr)
            {
                m_inst_draw_strategy = make_inst_render_strategy();
            }

            return m_inst_draw_strategy.get();
        }
    }
}