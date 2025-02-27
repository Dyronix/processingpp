#include "render/opengl/render_gl_api.h"

namespace ppp
{
    namespace render
    {
        namespace opengl
        {
            //-------------------------------------------------------------------------
            ifunction_library& api::instance()
            {
#if PPP_HEADLESS
                static function_library library;
#else
                static mock_function_library library;
#endif

                return library;
            }
        }
    }
}