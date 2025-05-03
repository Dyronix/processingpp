#include "engine.h"

#include "a-star-grid.h"
#include "a-star-grid-drawer.h"

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    constexpr int _rows = _window_width / 128;
    constexpr int _columns = _window_height / 72;
    constexpr int _cell_size = 25;
    constexpr int _padding = 6;

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

        shader(material::tags::unlit::color());

        vec2 grid_origin = {};

        grid_origin.x = (_window_width / 2.0f) - ((_grid.columns() / 2.0f) * (_cell_size + _padding));
        grid_origin.y = (_window_height / 2.0f) - ((_grid.rows() / 2.0f) * (_cell_size + _padding));
    }

    void draw()
    {
        background(200);

        grid_drawer::draw(grid);
    }
}