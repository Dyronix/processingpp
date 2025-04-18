#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"
#include "image.h"
#include "mathematics.h"
#include "random.h"

namespace ppp
{
    bool _toggle = true;

    constexpr int _min_division = 60;
    constexpr int _max_division = 80;

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    void draw_rect(float x, float y, float w, float h, float depth)
    {
        if (depth > 0)
        {
            // Recursive case
            if (_toggle)
            {
                // Vertical line
                float size_percentage = random(_min_division, _max_division) / 100.0f;

                print("Vertical Line: ", size_percentage);

                // left rect
                draw_rect(x, y, w * size_percentage, h, depth - 1);
                // right rect
                draw_rect(x + w * size_percentage, y, w * (1 - size_percentage), h, depth - 1);
            }
            else
            {
                // Horizontal line
                float size_percentage = random(_min_division, _max_division) / 100.0f;

                print("Horizontal Line: ", size_percentage);

                // top rect
                draw_rect(x, y, w, h * size_percentage, depth - 1);
                // bottom rect
                draw_rect(x, y + h * size_percentage, w, h * (1 - size_percentage), depth - 1);
            }

            _toggle = !_toggle;
        }
        else
        {
            // Base case: depth = 0
            // draw a rectangle with white fill or random color
            fill(255);
            std::vector<color> colors =
            {
                {235, 0, 0, 255},   // red
                {0, 0, 235, 255},   // blue
                {235, 235, 0, 255}  // yellow
            };

            float color_probability = random(0, 100) / 100.0f;
            if (color_probability < 0.3)
            {
                const color& c = random(colors);
                fill(c);
            }

            rect(x - 2.5f, y - 2.5f, w - 2.5f, h - 2.5f);
        }
    }

    app_params entry(int argc, char** argv)
    {
        app_params app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        return app_params;
    }

    void setup()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        add_mouse_pressed_callback(
            [](mouse_code code)
        {
            if (code == mouse_code::BUTTON_LEFT)
            {
                redraw();
            }
        });

        inner_stroke(0);
        inner_stroke_weight(5.0f);

        rect_mode(shape_mode_type::CORNER);
    }

    void draw()
    {   
        background(0);

        draw_rect(40, 40, _window_width - 80, _window_height - 80, 3);

        no_loop();
    }
}