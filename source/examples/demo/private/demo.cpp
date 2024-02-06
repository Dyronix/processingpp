#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"

namespace ppp
{
    AppParams entry()
    {
        AppParams app_params;

        app_params.window_width = 1280;
        app_params.window_height = 720;

        return app_params;
    }

    void setup()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);

        //rendering::create_canvas((1280 / 2) - 200, (720 / 2) - 200, 400.0f, 400.0f);

        color::background(15);
    }

    void draw()
    {
        transform::push();       
        {
            transform::translate(environment::canvas_width() / 2, environment::canvas_height() / 2);

            transform::push();
            {
                color::fill(255);

                transform::scale(2.0f, 2.0f);
                shapes::rect(0, 0, 100, 100);
            }
            transform::pop();
        }

        color::fill(255, 0, 0, 255);
        
        shapes::rect(0, 0, 100, 100);

        transform::pop();
    }
}