#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "util/log.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        std::string gl_get_error_message(u32 error)
        {
            switch (error)
            {
                case GL_INVALID_OPERATION: return "Invalid Operation";
                case GL_INVALID_ENUM: return "Invalid Enum";
                case GL_INVALID_VALUE: return "Invalid Value";
                case GL_INVALID_INDEX: return "Invalid Index";
                case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid Framebuffer Operation";
            }

            return "Unknown error";
        }

        //-------------------------------------------------------------------------
        u32 gl_check_error()
        {
            u32 error = GL_NO_ERROR;

            error = opengl::api::instance().get_error();  // no GL_CALL here to avoid recursive function call.

            return error;
        }

        //-------------------------------------------------------------------------
        void gl_flush_errors()
        {
            u32 error = gl_check_error();

            while (error != GL_NO_ERROR)
            {
                log::error("[OpenGL Error] flushing OpenGL error queue: ({0}, {1})", error, gl_get_error_message(error));
                error = gl_check_error();
            }
        }

        //-------------------------------------------------------------------------
        bool gl_log_call(const char* function, const char* file, s32 line)
        {
            u32 error = gl_check_error();
            if (error == GL_NO_ERROR)
            {
                return true;
            }

            while (error != GL_NO_ERROR)
            {
                log::error("[OpenGL Error] ({0}, {1}): {2} {3}: {4}", error, gl_get_error_message(error), function, file, line);

                error = gl_check_error();
            }

            return false;
        }
    }
}