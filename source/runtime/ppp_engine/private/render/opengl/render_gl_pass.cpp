#include "render/render_pass.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        render_pass::render_pass(const string::string_id shader_tag)
            :m_shader_tag(shader_tag)
        {}

        //-------------------------------------------------------------------------
        resources::shader_program render_pass::shader_program() const
        {
            auto m = material();
            auto p = shader_pool::get_shader_program(m->shader_tag());

            return p;
        }

        //-------------------------------------------------------------------------
        resources::imaterial* render_pass::material() const
        {
            return material_pool::material_at_shader_tag(m_shader_tag);
        }
    }
}