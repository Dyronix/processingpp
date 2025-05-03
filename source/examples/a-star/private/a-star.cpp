#include "engine.h"

#include "a-star-grid.h"

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    constexpr int _rows = _window_width / 64;
    constexpr int _columns = _window_height / 36;
    constexpr int _cell_size = 25;
    constexpr int _padding = 5;

    static grid _grid(_rows, _columns);

    app_params entry(int argc, char** argv)
    {
        print("Current working directory: %s", cwd().data());

        app_params app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        return app_params;
    }

    void setup()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        rect_mode(shape_mode_type::CENTER);
        ellipse_mode(shape_mode_type::CENTER);

        activate_camera(camera::tags::orthographic());
    }

    void draw()
    {
        background(200);

        float x = (_window_width / 2.0f) - ((_grid.columns() / 2.0f) * (_cell_size + _padding));
        float y = (_window_height / 2.0f) - ((_grid.rows() / 2.0f) * (_cell_size + _padding));

        fill(255, 0, 0, 255);
        circle(x, y, 10.0f);

        fill(0);
        stroke(255);
        for (int c = 0; c < _grid.columns(); ++c)
        {
            for (int r = 0; r < _grid.rows(); ++r)
            {
                rect(x, y, _cell_size, _cell_size);

                x += _cell_size + _padding;
            }

            x = (_window_width / 2.0f) - ((_grid.columns() / 2.0f) * (_cell_size + _padding));
            y += _cell_size + _padding;
        }
    }
}