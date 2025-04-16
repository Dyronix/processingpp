#include "render/render_shader.h"
#include "render/render_shader_compiler.h"

#include "render/opengl/render_gl_api.h"

#include "memory/memory_unique_ptr_util.h"

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            class shader_program::impl
            {
            public:
                //-------------------------------------------------------------------------
                impl(shading_model_type smtype, shading_blending_type btype, vertex_type vtype, std::string_view vs_source, std::string_view fs_source)
                {
                    program_id = create_shader_program(vs_source, fs_source);
                    shading_model_type = smtype;
                    shading_blending_type = btype;
                    vertex_type = vtype;
                }

                //-------------------------------------------------------------------------
                impl(shading_model_type smtype, shading_blending_type btype, vertex_type vtype, std::string_view vs_source, std::string_view fs_source, std::string_view geometry_source)
                {
                    program_id = create_shader_program(vs_source, fs_source, geometry_source);
                    shading_model_type = smtype;
                    shading_blending_type = btype;
                    vertex_type = vtype;
                }

                u32 program_id = 0;
                shading_model_type shading_model_type = shading_model_type::UNLIT;
                shading_blending_type shading_blending_type = shading_blending_type::OPAQUE;
                vertex_type vertex_type = vertex_type::POSITION_COLOR;
            };

            //-------------------------------------------------------------------------
            shader_program::shader_program(shading_model_type smtype, shading_blending_type btype, vertex_type vtype, std::string_view vs_source, std::string_view fs_source)
                :m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(smtype, btype, vtype, vs_source, fs_source))
            {
            }

            //-------------------------------------------------------------------------
            shader_program::shader_program(shading_model_type smtype, shading_blending_type btype, vertex_type vtype, std::string_view vs_source, std::string_view fs_source, std::string_view geometry_source)
                :m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(smtype, btype, vtype, vs_source, fs_source, geometry_source))
            {
            }

            //-------------------------------------------------------------------------
            shader_program::~shader_program()
            {
                opengl::api::instance().delete_program(m_pimpl->program_id);
            }

            //-------------------------------------------------------------------------
            void shader_program::bind() const
            {
                opengl::api::instance().use_program(m_pimpl->program_id);
            }

            //-------------------------------------------------------------------------
            void shader_program::unbind() const
            {
                opengl::api::instance().use_program(0);
            }

            //-------------------------------------------------------------------------
            u32 shader_program::id() const
            {
                return m_pimpl->program_id;
            }

            //-------------------------------------------------------------------------
            shading_model_type shader_program::shading_model() const
            {
                return m_pimpl->shading_model_type;
            }

            //-------------------------------------------------------------------------
            shading_blending_type shader_program::shading_blending() const
            {
                return m_pimpl->shading_blending_type;
            }

            //-------------------------------------------------------------------------
            vertex_type shader_program::vertex_format() const
            {
                return m_pimpl->vertex_type;
            }
        }
    }
}