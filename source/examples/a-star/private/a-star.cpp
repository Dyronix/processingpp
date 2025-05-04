#include "engine.h"

#include "a-star-grid.h"
#include "a-star-grid-drawer.h"

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    constexpr int _grid_width = _window_width / 64;
    constexpr int _grid_height = _window_height / 72;
    constexpr int _grid_cell_size = 50;

    static float grid_origin_x()
    {
        constexpr float half_window_width = _window_width / 2.0f;
        constexpr float half_rows = static_cast<float>(_grid_width) / 2.0f;
        constexpr float full_cell_size = _grid_cell_size;

        return half_window_width - (half_rows * full_cell_size);
    }
    static float grid_origin_y()
    {
        constexpr float half_window_height = _window_height / 2.0f;
        constexpr float half_columns = static_cast<float>(_grid_height) / 2.0f;
        constexpr float full_cell_size = _grid_cell_size;

        return half_window_height - (half_columns * full_cell_size);
    }

    static world_grid _grid({ grid_origin_x(), grid_origin_y() }, _grid_cell_size, _grid_width, _grid_height);

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

        text_font(load_font("local:/content/fonts/PokemonGb-RAeo.ttf"));

        rect_mode(shape_mode_type::CENTER);
        ellipse_mode(shape_mode_type::CENTER);

        activate_camera(camera::tags::orthographic());

        shader(material::tags::unlit::color());
    }

    void draw()
    {
        background(200);

        grid_drawer::draw(_grid);
    }
}