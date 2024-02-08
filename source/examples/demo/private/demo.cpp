#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"
#include "image.h"

namespace ppp
{
    image::Image _image;

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

        rendering::create_canvas((1280 / 2) - 200, (720 / 2) - 200, 400.0f, 400.0f);

        color::background(15);

        _image = image::load("blank.png");
    }

    void draw()
    {
        transform::push();       
        {
            transform::translate(environment::canvas_width() / 2, environment::canvas_height() / 2);

            //    transform::push();
            //    {
            //        color::fill(255);
            //        transform::scale(2.0f, 2.0f);
            //        shapes::rect(0, 0, 100, 100);
            //    }
            //    transform::pop();

            shapes::rect_mode(shapes::ShapeMode::CORNER);
            color::stroke_weight(5);
            color::stroke(0, 255, 0, 255);

            color::fill(255, 0, 0, 255);
            shapes::ellipse(-50, 0, 75, 75);
            shapes::rect(50, 0, 75, 75);
            shapes::triangle(0, 150, 25, 200, 50, 150);
            //shapes::rect(150, 0, 100, 100);
            //shapes::line(0, 0, 100, 100);
            //shapes::point(0, 0);

            image::draw(_image.id, 0, -100, 100, 100);
        }
        transform::pop();
    }
}