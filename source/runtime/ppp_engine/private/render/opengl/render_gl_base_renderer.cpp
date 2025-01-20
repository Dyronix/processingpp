#include "render/render_base_renderer.h"

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
        }

        //-------------------------------------------------------------------------
        base_renderer::base_renderer(const attribute_layout* layouts, u64 layout_count, const std::string& shader_tag)
            : m_user_shader_tag({})
            , m_shader_tag(shader_tag)
            , m_rasterization_mode(internal::_solid)
            , m_layouts(layouts)
            , m_layout_count(layout_count)
        {}

        //-------------------------------------------------------------------------
        base_renderer::~base_renderer() = default;

        //-------------------------------------------------------------------------
        void base_renderer::enable_solid_rendering(bool enable)
        {
            if (enable)
            {
                m_rasterization_mode |= internal::_solid;
            }
            else
            {
                m_rasterization_mode &= ~internal::_solid;
            }

            //// Make sure the solid_render function is either excluded or included in the render policy
            //draw_policy(m_render_policy);
        }
        //-------------------------------------------------------------------------
        void base_renderer::enable_wireframe_rendering(bool enable)
        {
            if (enable)
            {
                m_rasterization_mode |= internal::_wireframe;
            }
            else
            {
                m_rasterization_mode &= ~internal::_wireframe;
            }

            //// Make sure the wireframe_render function is either excluded or included in the render policy
            //draw_policy(m_render_policy);
        }

        //-------------------------------------------------------------------------
        void base_renderer::user_shader_program(const std::string& tag)
        {
            if (shader_pool::has_shader(tag))
            {
                m_user_shader_tag = tag;
            }
        }

        //-------------------------------------------------------------------------
        void base_renderer::reset_user_shader_program()
        {
            m_user_shader_tag = {};
        }

        //-------------------------------------------------------------------------
        bool base_renderer::solid_rendering_supported() const
        {
            return m_rasterization_mode & internal::_solid;
        }

        //-------------------------------------------------------------------------
        bool base_renderer::wireframe_rendering_supported() const
        {
            return m_rasterization_mode & internal::_wireframe;
        }

        //-------------------------------------------------------------------------
        u32 base_renderer::shader_program() const
        {
            resources::imaterial* m = material();
            
            return shader_pool::get_shader_program(m->shader_tag());
        }

        //-------------------------------------------------------------------------
        resources::imaterial* base_renderer::material() const
        {
            return material_pool::material_at_shader_tag(m_user_shader_tag.empty() ? m_shader_tag : m_user_shader_tag);
        }
    }
}