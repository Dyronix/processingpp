#include "render/render_pass.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "resources/framebuffer_pool.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        render_pass::render_pass(const string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags)
            :m_shader_tag(shader_tag)
            ,m_framebuffer_tag(framebuffer_tag)
            ,m_framebuffer_flags(framebuffer_flags)
        {}

        //-------------------------------------------------------------------------
        const resources::iframebuffer* render_pass::framebuffer() const
        {
            auto framebuffer = framebuffer_pool::get(m_framebuffer_tag, m_framebuffer_flags);

            return framebuffer;
        }

        //-------------------------------------------------------------------------
        const resources::shader_program render_pass::shader_program() const
        {
            auto m = material();
            auto p = shader_pool::get_shader_program(m->shader_tag());

            return p;
        }

        //-------------------------------------------------------------------------
        const resources::imaterial* render_pass::material() const
        {
            return material_pool::material_at_shader_tag(m_shader_tag);
        }

        //-------------------------------------------------------------------------
        ibatch_render_strategy* render_pass::batch_render_strategy()
        {
            if (m_batch_draw_strategy == nullptr)
            {
                m_batch_draw_strategy = make_batch_render_strategy();
            }

            return m_batch_draw_strategy.get();
        }
        //-------------------------------------------------------------------------
        iinstance_render_strategy* render_pass::instance_render_strategy()
        {
            if (m_inst_draw_strategy == nullptr)
            {
                m_inst_draw_strategy = make_inst_render_strategy();
            }

            return m_inst_draw_strategy.get();
        }
    }
}