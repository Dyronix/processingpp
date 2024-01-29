#include "environment.h"
#include "device.h"

namespace ppp
{
    namespace environment
    {
        f32 window_width()
        {
            s32 width = 0;
            device::window_width(&width);
            return static_cast<f32>(width);
        }

        f32 window_height()
        {
            s32 height = 0;
            device::window_height(&height);
            return static_cast<f32>(height);
        }
    }
}