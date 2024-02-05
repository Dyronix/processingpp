#include "environment.h"
#include "device/device.h"
#include "render/render.h"
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

        float canvas_width()
        {
            if (render::scissor_enabled())
            {
                return render::scissor().w;
            }

            return window_width();
        }

        float canvas_height()
        {
            if (render::scissor_enabled())
            {
                return render::scissor().h;
            }

            return window_height();
        }
    }
}