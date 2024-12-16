#include "entrypoint.h"
#include "environment.h"
#include "color.h"

#include "device/device.h"
#include "render/render.h"
#include "resources/texture_pool.h"
#include "resources/font_pool.h"
#include "resources/shader_pool.h"
#include "fileio/fileio.h"

#include "util/log.h"
#include "util/types.h"
#include "util/steady_clock.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ppp
{
    namespace internal
    {
        //-------------------------------------------------------------------------
        std::string get_working_directory(const std::string& executable_path)
        {
            size_t last_slash_pos = executable_path.find_last_of("/\\");

            return executable_path.substr(0, last_slash_pos + 1);
        }
    }

    //-------------------------------------------------------------------------
    s32 init(const app_params& app_params, const std::string& executable_path)
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

        if(!shader_pool::initialize())
        {
            return -1;
        }

        fileio::add_wildcard("local:", internal::get_working_directory(executable_path));

        color::background(1.0f, 1.0f, 1.0f, 1.0f);

        setup();

        return 0;
    }
    //-------------------------------------------------------------------------
    s32 run(const app_params& app_params)
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
            device::tick();
            device::poll_events();

            // force app to run at a certain framerate
            // ----
            if (device::is_looping())
            {
                if (device::delta_time() < device::max_frame_time())
                {
                    f32 sleep_time_seconds = device::max_frame_time() - device::delta_time();

                    // Convert seconds to milliseconds for sleeping
                    clock::milliseconds sleep_time(static_cast<s32>(sleep_time_seconds * 1000));
                    clock::accurate_sleep_for(sleep_time);
                }
            }
        }

        return 0;
    }
    //-------------------------------------------------------------------------
    s32 quit(const app_params& app_params)
    {
        font_pool::terminate();
        texture_pool::terminate();
        render::terminate();
        device::terminate();

        return 0;
    }

    //-------------------------------------------------------------------------
    bool has_argument(int argc, char** argv, const char* target)
    {
        return find_argument(argc, argv, target) != -1;
    }

    //-------------------------------------------------------------------------
    int find_argument(int argc, char** argv, const char* target)
    {
        for (int i = 1; i < argc; i++) 
        {
            if (strcmp(argv[i], target) == 0) 
            {
                return i;
            }
        }

        return -1;
    }

    //-------------------------------------------------------------------------
    int find_argument_with_value(int argc, char** argv, const char* target, const char** value)
    {
        for (int i = 1; i < argc; i++) 
        {
            if (strcmp(argv[i], target) == 0 && (i + 1) < argc) 
            {
                *value = argv[i + 1];
                return i;
            }
        }
        
        return -1;
    }
}

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; ++i)
    {
        ppp::log::info("Application argument: {}", argv[i]);
    }

    ppp::app_params app_params = ppp::entry(argc, argv);

    s32 result = 0;

    result = ppp::init(app_params, argv[0]);
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
