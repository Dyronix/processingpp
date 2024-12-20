#include "engine.h"

// Demo of shapes and images with stroke and inner stroke
namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    void setup_canvas()
    {
        color::background(15);
    }

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);
    }

    app_params entry(int argc, char** argv)
    {
        environment::print("current working dir: " + environment::cwd());

        app_params app_params;

        app_params.window_width = 1280;
        app_params.window_height = 720;

        return app_params;
    }

    void setup()
    {
        setup_canvas();
        setup_input_events();

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::set_scene_camera(20, -40, 400);
    }

    void draw()
    {
        color::background(200);

        camera::orbit_scene_camera_options options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        camera::orbit_control(options);

        color::fill({ 255,0,0,255 });
    }
}