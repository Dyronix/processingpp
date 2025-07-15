#include "entrypoint.h"
#include "framework.h"

#include "environment.h"

#include <cassert>

namespace ppp
{
    //-------------------------------------------------------------------------
    static void on_begin_frame();
    static void on_pre_render();
    static void on_post_render();
    static void on_tick();
    static void on_end_frame();

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    std::unique_ptr<sketch> _sketch = nullptr;

    //-------------------------------------------------------------------------
    app_params entry(int argc, char** argv)
    {
        print("Current working directory: %s", cwd().data());

        const char* resx;
        int in_window_width = -1;
        if (find_argument_with_value(argc, argv, "--resx", &resx))
        {
            try
            {
                in_window_width = std::stoi(resx);
            }
            catch (std::exception& e)
            {
                print(e.what());
            }
        }
        const char* resy;
        int in_window_height = -1;
        if (find_argument_with_value(argc, argv, "--resy", &resy))
        {
            try
            {
                in_window_height = std::stoi(resy);
            }
            catch (std::exception& e)
            {
                print(e.what());
            }
        }

        app_params app_params;

        app_params.window_width = in_window_width != -1 ? in_window_width : _window_width;
        app_params.window_height = in_window_height != -1 ? in_window_height : _window_height;

        return app_params;
    }

    //-------------------------------------------------------------------------
    void setup()
    {
        _sketch = make_sketch();

        assert(_sketch != nullptr);

        subscribe_begin_frame(&on_begin_frame);
        subscribe_pre_render(&on_pre_render);
        subscribe_post_render(&on_post_render);
        subscribe_step(&on_tick);
        subscribe_end_frame(&on_end_frame);

        _sketch->setup();
    }

    //-------------------------------------------------------------------------
    void on_begin_frame()
    {
        _sketch->begin_frame();
    }

    //-------------------------------------------------------------------------
    void on_pre_render()
    {
        _sketch->predraw();
    }

    //-------------------------------------------------------------------------
    void draw()
    {
        _sketch->draw();
    }

    //-------------------------------------------------------------------------
    void on_post_render()
    {
        _sketch->postdraw();
    }

    //-------------------------------------------------------------------------
    void on_tick()
    {
        _sketch->tick(delta_time());
    }

    //-------------------------------------------------------------------------
    void on_end_frame()
    {
        _sketch->end_frame();
    }
}

