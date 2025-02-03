#include "render/render_shader.h"
#include "render/render_shader_compiler.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {
            //-------------------------------------------------------------------------
            shader_program::shader_program(std::string_view vs_source, std::string_view fs_source)
            {
                m_program_id = create_shader_program(vs_source, fs_source);
            }

            //-------------------------------------------------------------------------
            shader_program::shader_program(std::string_view vs_source, std::string_view fs_source, std::string_view geometry_source)
            {
                m_program_id = create_shader_program(vs_source, fs_source, geometry_source);
            }

            //-------------------------------------------------------------------------
            shader_program::~shader_program()
            {
                glDeleteProgram(m_program_id);
            }

            //-------------------------------------------------------------------------
            void shader_program::bind() const
            {
                glUseProgram(m_program_id);
            }

            //-------------------------------------------------------------------------
            void shader_program::unbind() const
            {
                glUseProgram(0);
            }

            //-------------------------------------------------------------------------
            u32 shader_program::id() const
            {
                return m_program_id;
            }
        }
    }
}