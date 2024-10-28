#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "typography.h"
#include "environment.h"
#include "camera.h"
#include "mathematics.h"
#include "material.h"

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    typography::font_id _font;

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);
    }

    void load_font()
    {
        _font = typography::load_font("local:content/fonts/PokemonGb-RAeo.ttf", 18);
    }

    void activate_font(typography::font_id font)
    {
        typography::text_font(font);
    }

    AppParams entry()
    {
        AppParams app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        return app_params;
    }

    void setup()
    {
        setup_input_events();

        load_font();
        activate_font(_font);

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::camera(20, -40, 300);

        material::normal_material();
    }

    void draw()
    {
        color::background(200);

        camera::OrbitCameraOptions options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        camera::orbit_control(options);

        color::fill({ 255,0,0,255 });

        shapes::box(50.0f, 50.0f, 50.0f);

        //color::fill({0,0,0,255});

        //std::string str_frame_rate = "fps " + std::to_string(environment::average_frame_rate());
        //std::string str_delta_time = "ms " + std::to_string(environment::delta_time());

        //typography::text(str_frame_rate, 10, _window_height - 30);
        //typography::text("-", 135, _window_height - 30);
        //typography::text(str_delta_time, 170, _window_height - 30);
    }
}