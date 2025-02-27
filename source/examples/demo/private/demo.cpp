#include "engine.h"

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    image::image _image_container;
    image::image _image_wall;
    image::image _image_awesomeface;

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::key_code::KEY_ESCAPE);
    }

    app_params entry(int argc, char** argv)
    {
        environment::print("Current working directory: %s", environment::cwd().data());

        app_params app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        return app_params;
    }

    void setup()
    {
        setup_input_events();

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::set_scene_camera(20, -40, 400);

        _image_container = image::load("local:content/t_container.jpg");
        _image_wall = image::load("local:content/t_wall.jpg");
        _image_awesomeface = image::load("local:content/t_awesomeface.jpg");

        rendering::enable_batched_draw_mode();
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

        color::fill({ 255,255,255,255 });

        int grid_width = 2;
        int grid_height = 1;
        int grid_depth = 1;

        float cube_x_offset = 60.0f;
        float cube_y_offset = 60.0f;
        float cube_z_offset = 60.0f;

        float start_x = (-1.0f * cube_x_offset) * (grid_width / 2);
        float start_y = (-1.0f * cube_y_offset) * (grid_height / 2);
        float start_z = (-1.0f * cube_z_offset) * (grid_depth / 2);

        for (int layer = 0; layer < grid_depth; ++layer)
        {
            for (int row = 0; row < grid_height; ++row)
            {
                for (int col = 0; col < grid_width; ++col)
                {
                    transform::push();

                    material::reset_textures();
                    material::texture((layer + row + col) % 2 ? _image_wall.id : _image_container.id);
                    material::texture(_image_awesomeface.id);

                    transform::translate(
                        start_x + col * cube_x_offset,
                        start_y + row * cube_y_offset,
                        start_z + layer * cube_z_offset
                    );

                    shapes::box(50.0f, 50.0f, 50.0f);
                    transform::pop();
                }
            }
        }
    }
}