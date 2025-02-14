#include "entrypoint.h"
#include "environment.h"
#include "color.h"
#include "material.h"
#include "events.h"

#include "device/device.h"
#include "render/render.h"
#include "camera/camera_manager.h"

#include "memory/memory_tracker.h"

#include "memory/heaps/heap.h"

#include "resources/texture_pool.h"
#include "resources/font_pool.h"
#include "resources/shader_pool.h"
#include "resources/material_pool.h"
#include "resources/geometry_pool.h"
#include "resources/framebuffer_pool.h"

#include "fileio/fileio.h"
#include "fileio/vfs.h"

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
        std::string_view get_working_directory(std::string_view executable_path)
        {
            size_t last_slash_pos = executable_path.find_last_of("/\\");

            return executable_path.substr(0, last_slash_pos + 1);
        }
    }

    //-------------------------------------------------------------------------
    s32 init(const app_params& app_params, std::string_view executable_path)
    {
        if (!device::initialize(app_params.window_width, app_params.window_height))
        {
            log::error("Failed to initialize device");
            return -1;
        }

#if PPP_OPENGL
        if (!render::initialize(app_params.window_width, app_params.window_height, glfwGetProcAddress))
        {
            log::error("Failed to initialize render");
            return -1;
        }
#endif
        if (!camera_manager::initialize((f32)app_params.window_width, (f32)app_params.window_height))
        {
            log::error("Failed to initialize camera manager"); 
            return -1;
        }

        if (!framebuffer_pool::initialize(app_params.window_width, app_params.window_height))
        {
            log::error("Failed to initialize framebuffer pool");
            return -1;
        }

        if (!texture_pool::initialize())    { log::error("Failed to initialize texture pool");   return -1; }
        if (!font_pool::initialize())       { log::error("Failed to initialize font pool");      return -1; }
        if (!shader_pool::initialize())     { log::error("Failed to initialize shader pool");    return -1; }
        if (!material_pool::initialize())   { log::error("Failed to initialize material pool");  return -1; }
        if (!geometry_pool::initialize())   { log::error("Failed to initialize geometry pool");  return -1; }      

        auto unlit_texture = shader_pool::tags::unlit_texture();

        material::shader(string::restore_sid(unlit_texture));

        vfs::add_wildcard(string::store_sid("local:"), internal::get_working_directory(executable_path));

        color::background(1.0f, 1.0f, 1.0f, 1.0f);

#if _DEBUG
        keyboard::add_key_pressed_callback(
            [](keyboard::key_code key)
            {
                if (key == keyboard::key_code::KEY_F7)
                {
                    memory::peek();
                }
            });
#endif

        setup();

        memory::memory_manager::instance().get_tagged_heap()->free_blocks(memory::tags::fileio);
        memory::memory_manager::instance().get_tagged_heap()->free_blocks(memory::tags::initialization);

        return 0;
    }
    //-------------------------------------------------------------------------
    s32 run(const app_params& app_params)
    {
        render::render_context context;

        while (!device::should_close())
        {
            memory::start_frame(device::current_frame_index());

            if (device::can_draw())
            {
                context.mat_proj_font = camera_manager::get_proj(camera_manager::tags::font());
                context.mat_view_font = camera_manager::get_view(camera_manager::tags::font());
                context.mat_proj_active = camera_manager::get_proj();
                context.mat_view_active = camera_manager::get_view();

                // render
                // ------
                render::begin();

                draw();

                render::render(context);

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

            memory::memory_manager::instance().get_frame_heap()->free();
            memory::end_frame();
        }

        return 0;
    }
    //-------------------------------------------------------------------------
    s32 quit(const app_params& app_params)
    {
        geometry_pool::terminate();
        material_pool::terminate();
        shader_pool::terminate();
        font_pool::terminate();
        texture_pool::terminate();
        framebuffer_pool::terminate();
        camera_manager::terminate();
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
    ppp::memory::enable_tracking();

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

    ppp::memory::disable_tracking();

    return result;
}
