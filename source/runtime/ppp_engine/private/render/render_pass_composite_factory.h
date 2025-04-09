#pragma once

#include "render/render_pass_composite.h"
#include "render/render_pass_unlit.h"
#include "render/render_pass_unlit_wireframe.h"
#include "render/render_pass_forward_shading.h"
#include "render/render_pass_shadow.h"
#include "render/render_pass_predepth.h"
#include "render/render_shader_tags.h"

#include "string/string_id.h"

#include <memory>

namespace ppp
{
    namespace render
    {
        using unlit_composite_pass = std::unique_ptr<composite_pass<unlit_pass>>;
        using unlit_wireframe_composite_pass = std::unique_ptr<composite_pass<unlit_wireframe_pass>>;
        using forward_shading_composite_pass = std::unique_ptr<composite_pass<forward_shading_pass>>;
        using shadow_composite_pass = std::unique_ptr<composite_pass<shadow_pass>>;
        using predepth_composite_pass = std::unique_ptr<composite_pass<predepth_pass>>;

        //-------------------------------------------------------------------------
        template<typename T, typename TTag>
        static std::unique_ptr<composite_pass<T>> create_composite_pass(string::string_id pass_name, tag<TTag> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags)
        {
            auto composite = std::make_unique<composite_pass<T>>(pass_name);

            composite->add_child(std::make_unique<T>(decltype(shader_tag)::batched(), framebuffer_tag, framebuffer_flags, T::draw_mode::BATCHED));
            composite->add_child(std::make_unique<T>(decltype(shader_tag)::instanced(), framebuffer_tag, framebuffer_flags, T::draw_mode::INSTANCED));

            return composite;
        }

        //-------------------------------------------------------------------------
        template<typename TTag>
        unlit_composite_pass create_unlit_composite_pass(tag<TTag> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH)
        {
            return create_composite_pass<unlit_pass, TTag>("unlit_composite_pass"_sid, shader_tag, framebuffer_tag, framebuffer_flags);
        }

        //-------------------------------------------------------------------------
        template<typename TTag>
        unlit_wireframe_composite_pass create_unlit_wireframe_composite_pass(tag<TTag> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH)
        {
            return create_composite_pass<unlit_wireframe_pass, TTag>("unlit_wireframe_composite_pass"_sid, shader_tag, framebuffer_tag, framebuffer_flags);
        }

        //-------------------------------------------------------------------------
        template<typename TTag>
        forward_shading_composite_pass create_forward_shading_composite_pass(tag<TTag> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH)
        {
            return create_composite_pass<forward_shading_pass, TTag>("forward_composite_pass"_sid, shader_tag, framebuffer_tag, framebuffer_flags);
        }

        //-------------------------------------------------------------------------
        template<typename TTag>
        shadow_composite_pass create_shadow_composite_pass(tag<TTag> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::SAMPLED_DEPTH)
        {
            return create_composite_pass<shadow_pass, TTag>("shadow_composite_pass"_sid, shader_tag, framebuffer_tag, framebuffer_flags);
        }

        //-------------------------------------------------------------------------
        template<typename TTag>
        predepth_composite_pass create_predepth_composite_pass(tag<TTag> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH)
        {
            return create_composite_pass<predepth_pass, TTag>("predepth_composite_pass"_sid, shader_tag, framebuffer_tag, framebuffer_flags);
        }
    }
}