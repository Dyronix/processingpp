#include "entrypoint.h"
#include "agents.h"
#include "shapes.h"
#include "environment.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "image.h"

#include <filesystem>

namespace ppp
{
    namespace internal
    {
        image::image_id _uv_checker;
        image::image_id _checker;
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
        internal::_uv_checker = image::load("./uv-checker.png");
        internal::_checker = image::load("./checker.png");

        keyboard::add_key_pressed_callback(
            [](keyboard::KeyCode key)
        {
            if (key == keyboard::KeyCode::KEY_ESCAPE)
            {
                structure::quit();
            }
        });

        rendering::create_canvas(100, 100, environment::window_width() - 200, environment::window_height() - 200);

        color::background(0.2f * 255, 0.3f * 255, 0.3f * 255, 1.0f * 255);
    }

    void draw()
    {
        //color::fill(255, 0, 0, 255);
        //shapes::rect(-440, 0, 100.0f, 100.0f);

        //color::fill(0, 255, 0, 255);
        //shapes::rect(0, 0, 100.0f, 100.0f);

        //color::fill(0, 0, 255, 255);
        //shapes::rect(440, 0, 100.0f, 100.0f);

        //color::fill(255, 255, 255, 255);
        //for (int x = -environment::window_width() / 4; x < environment::window_width() / 4; x += 20)
        //{
        //    for (int y = -environment::window_height() / 4; y < environment::window_height() / 4; y += 20)
        //    {
        //        shapes::line(x, y, x + 15.0f, y);
        //    }
        //}

        image::draw(internal::_uv_checker, 0, 200, 100.0f, 100.0f);
        image::draw(internal::_uv_checker, 0, 0, 100.0f, 100.0f);
        image::draw(internal::_checker, 200, 0, 100.0f, 100.0f);
    }
}