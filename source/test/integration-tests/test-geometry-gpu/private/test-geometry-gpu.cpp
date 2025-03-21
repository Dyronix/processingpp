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

    typography::font_id _font;

    material::shader_program _material_cyber_ocean;

    // Separate amplitudes for X and Z directions
    constexpr int _amplitude_x = 50; // height of the wave in the x-direction
    constexpr int _amplitude_z = 20;  // height of the wave in the z-direction

    // Separate movement speeds for X and Z directions
    constexpr float _movement_speed_x = 0.5f; // speed of the wave movement in x-direction
    constexpr float _movement_speed_z = 0.5f; // speed of the wave movement in z-direction

    // Separate periodic scales for X and Z directions (frequency of the wave)
    constexpr float _periodic_scale_x = 0.1f; // how tight the waves are in x-direction
    constexpr float _periodic_scale_z = 0.1f; // how tight the waves are in z-direction

    // colors for the top and bottom cubes
    color::Color _color_top;
    color::Color _color_bottom; 

    // Grid properties
    constexpr int _grid_rows = 100; // Number of rows (Z direction)
    constexpr int _grid_cols = 100; // Number of columns (X direction)
    constexpr float _grid_spacing = 60.0f; // Distance between points

    void generate_cyber_ocean_grid()
    {
        std::vector<glm::vec3> points;

        for (int z = 0; z < _grid_rows; ++z) 
        {
            for (int x = 0; x < _grid_cols; ++x) 
            {
                float x_pos = (x - _grid_cols / 2.0f) * _grid_spacing;  // Center the grid at (0, 0, 0)
                float z_pos = (z - _grid_rows / 2.0f) * _grid_spacing;  // Center the grid at (0, 0, 0)

                shapes::point(x_pos, 0.0f, z_pos);
            }
        }
    }

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::key_code::KEY_ESCAPE);
    }

    void load_cyber_ocean_shader()
    {
        std::string vs_path = "local:/content/shaders/cyber_ocean.vs";
        std::string ps_path = "local:/content/shaders/cyber_ocean.fs";
        std::string gs_path = "local:/content/shaders/cyber_ocean.gs";

        _material_cyber_ocean = material::load_shader("cyber_ocean", vs_path, ps_path, gs_path);

        _material_cyber_ocean.set_uniform("u_amplitude_x", _amplitude_x);
        _material_cyber_ocean.set_uniform("u_amplitude_z", _amplitude_z);
        _material_cyber_ocean.set_uniform("u_movement_speed_x", _movement_speed_x);
        _material_cyber_ocean.set_uniform("u_movement_speed_z", _movement_speed_z);
        _material_cyber_ocean.set_uniform("u_periodic_scale_x", _periodic_scale_x);
        _material_cyber_ocean.set_uniform("u_periodic_scale_z", _periodic_scale_z);
        _material_cyber_ocean.set_uniform("u_total_time", environment::total_time());
        _material_cyber_ocean.set_uniform("u_cube_size", 20.0f);

        material::shader("cyber_ocean");
    }

    void create_cyber_ocean_geometry()
    {
        shapes::build_custom_geometry("cyber_ocean", &generate_cyber_ocean_grid);
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
                    environment::print("[TEST FAILED][GGPU] image buffers are not identical!");
                }
                else
                {
                    environment::print("[TEST SUCCESS][GGPU] image buffers are identical.");
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

        load_cyber_ocean_shader();
        create_cyber_ocean_geometry();

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::set_scene_camera(20, -40, 300);

        _color_top = { 255, 0, 0, 255 };     // Red for top
        _color_bottom = { 0, 0, 255, 255 };  // Blue for bottom

        structure::on_draw_end(end_draw);
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

        _material_cyber_ocean.set_uniform("u_total_time", _animate_scene ? environment::total_time() : 0);

        color::fill({0,0,0,255});
    }
}