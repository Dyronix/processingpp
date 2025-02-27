#include "engine.h"

#include <sstream>

namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _animate_scene = false;
    bool _no_testing = false;

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    image::image _image_container;
    image::image _image_wall;

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::key_code::KEY_ESCAPE);
    }

    void end_draw()
    {
        if (environment::frame_count() == 5)
        {
            std::stringstream stream;

            stream << "local:/";
            stream << TEST_PROJECT_NAME;
            stream << ".png";

            if (_generate_new_data)
            {
                image::load_pixels(0, 0, _window_width, _window_height);
                image::save_pixels(stream.str(), _window_width, _window_height);
            }

            if (!_no_testing)
            {
                auto test_frame = image::load(stream.str());
                auto test_frame_pixels = image::load_pixels(test_frame.id);

                size_t total_size = test_frame.width * test_frame.height * test_frame.channels;

                std::vector<unsigned char> active_test_frame_pixels(total_size);
                memcpy_s(
                    active_test_frame_pixels.data(),
                    total_size,
                    test_frame_pixels,
                    total_size);

                auto frame_pixels = image::load_pixels(0, 0, _window_width, _window_height);

                std::vector<unsigned char> active_frame_pixels(total_size);
                memcpy_s(
                    active_frame_pixels.data(),
                    total_size,
                    frame_pixels,
                    total_size);

                if (memcmp(active_test_frame_pixels.data(), active_frame_pixels.data(), total_size) != 0)
                {
                    environment::print("[TEST FAILED][RINSTTEX] image buffers are not identical!");
                }
                else
                {
                    environment::print("[TEST SUCCESS][RINSTTEX] image buffers are identical.");
                }
            }

            if (!_no_close_after_x_frames)
            {
                structure::quit();
            }
        }
    }

    app_params entry(int argc, char** argv)
    {
        environment::print("Current working directory: %s", environment::cwd().data());

        app_params app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        _generate_new_data = has_argument(argc, argv, "--generate-new-data");
        _no_close_after_x_frames = has_argument(argc, argv, "--no-close");
        _animate_scene = has_argument(argc, argv, "--animate");
        _no_testing = has_argument(argc, argv, "--no-testing");

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

        structure::on_draw_end(end_draw);

        rendering::enable_instance_draw_mode();

        material::shader(material::tags::unlit_texture());
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


        int grid_width = 5;
        int grid_height = 3;
        int grid_depth = 4;

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