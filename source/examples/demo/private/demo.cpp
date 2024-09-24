#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"
#include "image.h"
#include "typography.h"

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    typography::font_id _font;

    void setup_canvas()
    {
        rendering::create_canvas((_window_width / 2) - (_canvas_width / 2), (_window_height / 2) - (_canvas_height / 2), _canvas_width, _canvas_height);

        color::background(15);
    }

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);

        mouse::add_mouse_pressed_callback(
            [](mouse::MouseCode code)
        {
            if (code == mouse::MouseCode::BUTTON_LEFT)
            {
                structure::redraw();
            }
        });
    }

    void load_font()
    {
        _font = typography::load_font("fonts/PokemonGb-RAeo.ttf");
    }

    void activate_font()
    {
        typography::text_font(_font);
    }

    AppParams entry()
    {
        AppParams app_params;

        app_params.window_width = 1280;
        app_params.window_height = 720;

        return app_params;
    }

    void setup()
    {
        setup_canvas();
        setup_input_events();

        load_font();
        activate_font();

        shapes::rect_mode(shapes::ShapeMode::CORNER);
    }

    void draw()
    {
        //structure::no_loop();

        typography::text("Hello World", 10, 10);
    }
}