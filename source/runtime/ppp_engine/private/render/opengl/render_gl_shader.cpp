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
            shader_program::shader_program(const std::string& vs_source, const std::string& fs_source)
            {
                m_program_id = create_shader_program(vs_source.c_str(), fs_source.c_str());
            }

            //-------------------------------------------------------------------------
            shader_program::shader_program(const std::string& vs_source, const std::string& fs_source, const std::string& geometry_source)
            {
                m_program_id = create_shader_program(vs_source.c_str(), fs_source.c_str(), geometry_source.c_str());
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