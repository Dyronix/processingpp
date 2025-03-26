#include "render/render_base_renderer.h"

#include "render/helpers/render_vertex_layouts.h"

#include "render/opengl/render_gl_error.h"

#include "resources/shader_pool.h"
#include "resources/material_pool.h"

#include "util/log.h"
#include "util/color_ops.h"



#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static constexpr s32 _solid = 1 << 1;
            static constexpr s32 _wireframe = 1 << 0;

            //-------------------------------------------------------------------------
            const attribute_layout* get_attribute_layout(string::string_id shader_tag)
            {
                auto shader_program = shader_pool::get_shader_program(shader_tag);

                return fill_user_layout(shader_program->vertex_format());
            }
            //-------------------------------------------------------------------------
            u64 get_attribute_layout_count(string::string_id shader_tag)
            {
                auto shader_program = shader_pool::get_shader_program(shader_tag);

                return fill_user_layout_count(shader_program->vertex_format());
            }
        }

        class base_renderer::impl
        {
        public:
            impl(string::string_id tag)
                : user_shader_tag()
                , shader_tag(tag)
                , rasterization_mode(internal::_solid)
                , layouts(nullptr)
                , layout_count(-1)
            {}

            string::string_id           user_shader_tag;
            string::string_id           shader_tag;

            s32                         rasterization_mode;

            const attribute_layout*     layouts;
            u64                         layout_count;
        };

        //-------------------------------------------------------------------------
        base_renderer::base_renderer(string::string_id shader_tag)
            : m_pimpl(std::make_unique<impl>(shader_tag))
        {}

        //-------------------------------------------------------------------------
        base_renderer::~base_renderer() = default;

        //-------------------------------------------------------------------------
        void base_renderer::enable_solid_rendering(bool enable)
        {
            if (enable)
            {
                m_pimpl->rasterization_mode |= internal::_solid;
            }
            else
            {
                m_pimpl->rasterization_mode &= ~internal::_solid;
            }

            //// Make sure the solid_render function is either excluded or included in the render policy
            //draw_policy(m_render_policy);
        }
        //-------------------------------------------------------------------------
        void base_renderer::enable_wireframe_rendering(bool enable)
        {
            if (enable)
            {
                m_pimpl->rasterization_mode |= internal::_wireframe;
            }
            else
            {
                m_pimpl->rasterization_mode &= ~internal::_wireframe;
            }

            //// Make sure the wireframe_render function is either excluded or included in the render policy
            //draw_policy(m_render_policy);
        }

        //-------------------------------------------------------------------------
        bool base_renderer::solid_rendering_supported() const
        {
            return m_pimpl->rasterization_mode & internal::_solid;
        }

        //-------------------------------------------------------------------------
        bool base_renderer::wireframe_rendering_supported() const
        {
            return m_pimpl->rasterization_mode & internal::_wireframe;
        }

        //-------------------------------------------------------------------------
        resources::shader_program base_renderer::shader_program() const
        {
            auto m = material();
            auto p = shader_pool::get_shader_program(m->shader_tag());

            return p;
        }

        //-------------------------------------------------------------------------
        resources::imaterial* base_renderer::material() const
        {
            return material_pool::material_at_shader_tag(m_pimpl->user_shader_tag.is_none() ? m_pimpl->shader_tag : m_pimpl->user_shader_tag);
        }

        //-------------------------------------------------------------------------
        s32 base_renderer::rasterization_mode() const 
        {
            return m_pimpl->rasterization_mode;
        }

        //-------------------------------------------------------------------------
        const attribute_layout* base_renderer::layouts() const 
        {
            if (m_pimpl->layouts == nullptr)
            {
                m_pimpl->layouts = internal::get_attribute_layout(m_pimpl->shader_tag);
            }

            return m_pimpl->layouts;
        }
        //-------------------------------------------------------------------------
        u64 base_renderer::layout_count() const 
        {
            if (m_pimpl->layout_count == -1)
            {
                m_pimpl->layout_count = internal::get_attribute_layout_count(m_pimpl->shader_tag);
            }

            return m_pimpl->layout_count;
        }
    }
}