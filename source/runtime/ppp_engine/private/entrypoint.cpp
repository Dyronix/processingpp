#include "entrypoint.h"
#include "device.h"
#include "render.h"
#include "environment.h"
#include "log.h"

#include <iostream>

#include <GLFW/glfw3.h>

namespace ppp
{
    //-------------------------------------------------------------------------
    int rex_entry()
    {
        AppParams app_params = ppp::app_entry();

        if(!device::initialize(app_params.window_width, app_params.window_height))
        {
            return -1;
        }

    #if PPP_OPENGL
        device::set_framebuffer_resize_callback(render::viewport);

        if (!render::initialize(app_params.window_width, app_params.window_height, glfwGetProcAddress))
        {
            return -1;
        }
    #endif

        render::clear_color(0.2f, 0.3f, 0.3f, 1.0f);

        if (app_params.app_setup_func != nullptr)
        {
            app_params.app_setup_func();
        }

        // render loop
        // -----------
        while (!device::should_close())
        {
            // input
            // -----
            device::process_input();
 
            // render
            // ------
            render::begin();
            render::viewport(0, 0, environment::window_width(), environment::window_height());
            render::clear(render::COLOR_BUFFER_BIT);

            if (app_params.app_draw_func != nullptr)
            {
                app_params.app_draw_func();
            }

            render::render();
            render::end();

            // swap front/back buffers & poll new window events
            // -----
            device::present();
            device::poll_events();
        }

        render::terminate();
        device::terminate();

        return 0;
    }
}

int main()
{
    return ppp::rex_entry();
}
