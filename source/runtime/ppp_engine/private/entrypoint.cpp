#include "entrypoint.h"
#include "environment.h"
#include "color.h"

#include "device/device.h"
#include "render/render.h"
#include "resources/texture_pool.h"
#include "resources/font_pool.h"

#include "util/log.h"
#include "util/types.h"

#include <iostream>

#include <GLFW/glfw3.h>

namespace ppp
{
    //-------------------------------------------------------------------------
    s32 init(const AppParams& app_params)
    {
        if (!device::initialize(app_params.window_width, app_params.window_height))
        {
            return -1;
        }

        #if PPP_OPENGL
        if (!render::initialize(app_params.window_width, app_params.window_height, glfwGetProcAddress))
        {
            return -1;
        }
        #endif

        if (!texture_pool::initialize())
        {
            return -1;
        }

        if(!font_pool::initialize())
        {
            return -1;
        }

        color::background(1.0f, 1.0f, 1.0f, 1.0f);

        setup();

        return 0;
    }
    //-------------------------------------------------------------------------
    s32 run(const AppParams& app_params)
    {
        while (!device::should_close())
        {
            if (device::can_draw())
            {
                // render
                // ------
                render::begin();

                draw();

                render::render();
                render::end();

                // swap front/back buffers
                // -----
                device::present();
            }

            // poll new window events
            // ----
            device::poll_events();
        }

        return 0;
    }
    //-------------------------------------------------------------------------
    s32 quit(const AppParams& app_params)
    {
        font_pool::terminate();
        texture_pool::terminate();
        render::terminate();
        device::terminate();

        return 0;
    }
}

int main()
{
    ppp::AppParams app_params = ppp::entry();

    s32 result = 0;

    result = init(app_params);
    if (result != 0)
    {
        ppp::log::error("Failed to initialize app");
        return result;
    }
    result = run(app_params);
    if (result != 0)
    {
        ppp::log::error("Encountered runtime error in app");
        return result;
    }
    result = quit(app_params);
    if (result != 0)
    {
        ppp::log::error("Failed to quit app");
        return result;
    }

    return result;
}
