#include "render/render_context.h"
#include "render/render_pass.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "resources/framebuffer_pool.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        framebuffer_render_pass::framebuffer_render_pass(const string::string_id pass_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags)
            : irender_pass(pass_tag)
            , m_framebuffer_tag(framebuffer_tag)
            , m_framebuffer_flags(framebuffer_flags)
        {}

        //-------------------------------------------------------------------------
        framebuffer_render_pass::~framebuffer_render_pass() = default;

        //-------------------------------------------------------------------------
        const resources::iframebuffer* framebuffer_render_pass::framebuffer() const
        {
            auto framebuffer = framebuffer_pool::get(m_framebuffer_tag, m_framebuffer_flags);

            return framebuffer;
        }

        //-------------------------------------------------------------------------
        string::string_id framebuffer_render_pass::framebuffer_tag() const
        {
            return m_framebuffer_tag;
        }

        //-------------------------------------------------------------------------
        u32 framebuffer_render_pass::framebuffer_flags() const
        {
            return m_framebuffer_flags;
        }

        //-------------------------------------------------------------------------
        geometry_render_pass::geometry_render_pass(const string::string_id pass_tag, const string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags, draw_mode draw_mode)
            : framebuffer_render_pass(pass_tag, framebuffer_tag, framebuffer_flags)
            , m_shader_tag(shader_tag)
            , m_draw_mode(draw_mode)
        {}

        //-------------------------------------------------------------------------
        geometry_render_pass::~geometry_render_pass() = default;

        //-------------------------------------------------------------------------
        bool geometry_render_pass::should_render(const render_context& context) const
        {
            bool can_draw = true;

            if (m_draw_mode == draw_mode::BATCHED || m_draw_mode == draw_mode::AUTO)
            {
                can_draw &= context.batch_data->empty() == false;
            }

            if (m_draw_mode == draw_mode::INSTANCED || m_draw_mode == draw_mode::AUTO)
            {
                can_draw &= context.instance_data->empty() == false;
            }

            return can_draw;
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
        bool geometry_render_pass::batch_rendering_enabled() const
        {
            return m_draw_mode == draw_mode::BATCHED || m_draw_mode == draw_mode::AUTO;
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
        //-------------------------------------------------------------------------
        bool geometry_render_pass::instance_rendering_enabled() const
        {
            return m_draw_mode == draw_mode::INSTANCED || m_draw_mode == draw_mode::AUTO;
        }
    }
}