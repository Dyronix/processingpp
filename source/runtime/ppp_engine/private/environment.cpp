#include "environment.h"
#include "device/device.h"
#include "util/log.h"
#include "util/types.h"

namespace ppp
{
    namespace environment
    {
        void print(const std::string& message)
        {
            log::info(message);
        }

        float window_width()
        {
            s32 width = 0;
            device::window_width(&width);
            return static_cast<f32>(width);
        }

        float window_height()
        {
            s32 height = 0;
            device::window_height(&height);
            return static_cast<f32>(height);
        }
    }
}