#include "entrypoint.h"
#include "framework.h"
#include "environment.h"

#include "device/device.h"
#include "device/device_imgui.h"
#include "device/device_input_framework.h"

#include "render/render_imgui.h"

#include "resources/texture_reloader.h"

#include "imgui/imgui.h"
#include "imgui/inspector/inspector.h"
#include "imgui/inspector/inspector_profiler.h"

#include "util/log.h"

#include <cassert>

namespace
{
    bool _should_pause = false;
    bool _should_progress_to_next_frame = false;

    //-------------------------------------------------------------------------
    bool is_application_paused()
    {
        bool t = _should_pause && !_should_progress_to_next_frame;
        _should_progress_to_next_frame = false;
        return t;
    }

    //-------------------------------------------------------------------------
    // ImGUI
    static void setup_imgui()
    {
        ppp::imgui::init_imgui_context();
        ppp::imgui::init_imgui_renderer();
    }
    //-------------------------------------------------------------------------
    static void destroy_imgui()
    {
        ppp::imgui::shutdown_imgui_renderer();
        ppp::imgui::shutdown_imgui_context();
    }

    //-------------------------------------------------------------------------
    // ImGUI context
    static void step_imgui_context()
    {
        ppp::imgui::step_imgui_context();
    }
    //-------------------------------------------------------------------------
    static void input_mask_imgui_context()
    {
        ImGuiIO& io = ImGui::GetIO();

        ppp::device::capture_mask({ io.WantCaptureMouse, io.WantCaptureKeyboard });
    }

    //-------------------------------------------------------------------------
    // ImGUI renderer
    static void begin_imgui_renderer()
    {
        ppp::imgui::begin_draw_imgui_renderer();
    }
    //-------------------------------------------------------------------------
    static void end_imgui_renderer()
    {
        ppp::imgui::end_draw_imgui_renderer();
    }

    //-------------------------------------------------------------------------
    // ImGUI Inspector
    static void setup_inspector()
    {
        ppp::imgui::inspector::initialize();

        ppp::imgui::inspector::subscribe_pause([](bool value)
        {
            _should_pause = value;
            ppp::imgui::inspector::notify(ppp::imgui::inspector::notification_type::INFO, "Application paused", 1.0f);
        });
        ppp::imgui::inspector::subscribe_unpause([](bool value)
        {
            _should_pause = value;
            ppp::imgui::inspector::notify(ppp::imgui::inspector::notification_type::INFO, "Application resumed", 1.0f);
        });
        ppp::imgui::inspector::subscribe_next_frame([](bool value)
        {
            _should_progress_to_next_frame = value;
        });
        ppp::imgui::inspector::subscribe_reload_images([]()
        {
            s32 count = ppp::texture_reloader::reload();
            
            if (count > 0)
            {
                ppp::imgui::inspector::notify(ppp::imgui::inspector::notification_type::INFO, "Images " + std::to_string(count) + " reloaded", 1.0f);
            }
            else
            {
                ppp::imgui::inspector::notify(ppp::imgui::inspector::notification_type::INFO, "No images reloaded", 1.0f);
            }
        });

        ppp::imgui::inspector::set_pause_shortcut(ppp::key_code::KEY_P);
        ppp::imgui::inspector::set_nextframe_shortcut(ppp::key_code::KEY_F10);
    }
    //-------------------------------------------------------------------------
    static void destroy_inspector()
    {
        ppp::imgui::inspector::shutdown();
    }
    //-------------------------------------------------------------------------
    static void draw_inspector(f32 dt)
    {
        ppp::imgui::inspector::render(dt);
    }
}

namespace ppp
{
    //-------------------------------------------------------------------------
    static void on_begin_frame();
    static void on_pre_render();
    static void on_post_render();
    static void on_tick();
    static void on_end_frame();

#ifdef _DEBUG
    bool _enable_inspector;
#endif

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    std::unique_ptr<sketch> _sketch = nullptr;

    //-------------------------------------------------------------------------
    app_params entry(int argc, char** argv)
    {
        print("Current working directory: %s", cwd().data());

        const char* resx;
        int in_window_width = -1;
        if (find_argument_with_value(argc, argv, "--resx", &resx) != -1)
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
        if (find_argument_with_value(argc, argv, "--resy", &resy) != -1)
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

#ifdef _DEBUG
        _enable_inspector = has_argument(argc, argv, "--inspector");
#endif

        app_params app_params;

        app_params.window_width = in_window_width != -1 ? in_window_width : _window_width;
        app_params.window_height = in_window_height != -1 ? in_window_height : _window_height;

        return app_params;
    }

    //-------------------------------------------------------------------------
    void setup()
    {
#ifdef _DEBUG

        setup_imgui();

        device::reroute_input_callbacks();

        if (_enable_inspector)
        {
            setup_inspector();
        }
        else
        {
            log::info("Inspector disabled, use --inspector when launching to enable the inspector.");
        }
#endif

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
        _sketch->pre_draw();
    }

    //-------------------------------------------------------------------------
    void draw()
    {
        _sketch->draw();
    }

    //-------------------------------------------------------------------------
    void on_post_render()
    {
        _sketch->post_draw();

        #ifdef _DEBUG
        begin_imgui_renderer();

        // Block any input control that would be used by ImGUI
        input_mask_imgui_context();

        _sketch->debug_draw();

        if (_enable_inspector)
        {
            draw_inspector(delta_time());
        }

        end_imgui_renderer();
        #endif
    }

    //-------------------------------------------------------------------------
    void on_tick()
    {
        if (!is_application_paused())
        {
            _sketch->tick(delta_time());
        }

        step_imgui_context();
    }

    //-------------------------------------------------------------------------
    void on_end_frame()
    {
        _sketch->end_frame();
    }

    //-------------------------------------------------------------------------
    void on_shutdown()
    {
        _sketch->shutdown();

#ifdef _DEBUG
        if (_enable_inspector)
        {
            destroy_inspector();
        }

        destroy_imgui();
#endif
    }
}

