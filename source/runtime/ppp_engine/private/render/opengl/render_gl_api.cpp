#include "render/opengl/render_gl_api.h"
#include "device/device.h"
namespace ppp
{
    namespace render
    {
        namespace opengl
        {
            //-------------------------------------------------------------------------
            ifunction_library& api::instance()
            {
                if (device::is_headless())
                {
                    static mock_function_library library;
                    return library;
                }

                static function_library library;
                return library;
            }
        }
    }
}