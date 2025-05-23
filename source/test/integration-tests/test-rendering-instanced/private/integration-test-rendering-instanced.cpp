#include "engine.h"

#include <sstream>

namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _animate_scene = false;
    bool _no_testing = false;

    constexpr int _window_width = 240;
    constexpr int _window_height = 160;

    int _cols = 11; // number of columns in the grid (x-direction)
    int _rows = 9; // number of rows in the grid (z-direction)

    // Separate amplitudes for X and Z directions
    constexpr int _amplitude_x = 50; // height of the wave in the x-direction
    constexpr int _amplitude_z = 20;  // height of the wave in the z-direction

    // Separate movement speeds for X and Z directions
    constexpr float _movement_speed_x = 0.025f; // speed of the wave movement in x-direction
    constexpr float _movement_speed_z = 0.015f; // speed of the wave movement in z-direction

    // Separate periodic scales for X and Z directions (frequency of the wave)
    constexpr float _periodic_scale_x = 0.3f; // how tight the waves are in x-direction
    constexpr float _periodic_scale_z = 0.3f; // how tight the waves are in z-direction

    // colors for the top and bottom cubes
    color _color_top;
    color _color_bottom;

    void setup_input_events()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        add_key_pressed_callback(
            [](key_code key)
        {
            if (key == key_code::KEY_UP)
            {
                _rows += 1;
            }
            else if (key == key_code::KEY_DOWN)
            {
                _rows -= 1;
            }

            if (key == key_code::KEY_RIGHT)
            {
                _cols += 1;
            }
            else if (key == key_code::KEY_LEFT)
            {
                _cols -= 1;
            }
        });
    }

    void end_draw()
    {
        if (frame_count() == 5)
        {
            std::stringstream stream;

            stream << "local:/";
            stream << TEST_PROJECT_NAME;
            stream << ".png";

            if (_generate_new_data)
            {
                load_pixels(0, 0, _window_width, _window_height);
                save_pixels(stream.str(), _window_width, _window_height);
            }

            if (!_no_testing)
            {
                auto test_frame = load(stream.str());
                auto test_frame_pixels = load_pixels(test_frame.id);

                size_t total_size = test_frame.width * test_frame.height * test_frame.channels;

                std::vector<unsigned char> active_test_frame_pixels(total_size);
                memcpy_s(
                    active_test_frame_pixels.data(),
                    total_size,
                    test_frame_pixels,
                    total_size);

                auto frame_pixels = load_pixels(0, 0, _window_width, _window_height);

                std::vector<unsigned char> active_frame_pixels(total_size);
                memcpy_s(
                    active_frame_pixels.data(),
                    total_size,
                    frame_pixels,
                    total_size);

                if (memcmp(active_test_frame_pixels.data(), active_frame_pixels.data(), total_size) != 0)
                {
                    print("[TEST FAILED][RINST] image buffers are not identical!");
                }
                else
                {
                    print("[TEST SUCCESS][RINST] image buffers are identical.");
                }
            }

            if (!_no_close_after_x_frames)
            {
                quit();
            }
        }
    }

    app_params entry(int argc, char** argv)
    {
        print("Current working directory: %s", cwd().data());

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

        enable_wireframe_mode(false);
        enable_solid_mode(true);

        perspective(55.0f, _window_width / _window_height, 0.1f, 1000.0f);
        set_scene_camera(20, -40, 600);

        _color_top = { 255, 0, 0, 255 };     // Red for top
        _color_bottom = { 0, 0, 255, 255 };  // Blue for bottom

        on_draw_end(end_draw);

        enable_instance_draw_mode();

        shader(material::tags::unlit::color());
    }

    void draw()
    {
        background(200);

        orbit_control_options options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        orbit_control(options);

        // Calculate wave offsets based on frame count
        float wave_offset_x = _animate_scene ? frame_count() * _movement_speed_x : 0.0f;
        float wave_offset_z = _animate_scene ? frame_count() * _movement_speed_z : 0.0f;

        // Loop over grid of cubes (XZ plane)
        for (int x = 0; x < _cols; x++)
        {
            for (int z = 0; z < _rows; z++)
            {

                // Calculate the y position as a wave based on x and z positions
                // The y position is now a combination of two sine waves
                float y = sin((x * _periodic_scale_x) + wave_offset_x) * _amplitude_x
                    + sin((z * _periodic_scale_z) + wave_offset_z) * _amplitude_z;

                // Map the y position between the min and max amplitudes for color interpolation
                int min_amplitude = -_amplitude_x - _amplitude_z;
                int max_amplitude = _amplitude_x + _amplitude_z;

                // Color interpolation based on y position
                float color_interp = math::map(y, (float)min_amplitude, (float)max_amplitude, 0.0f, 1.0f);
                color cube_color = lerp_color(_color_bottom, _color_top, color_interp);
                fill(cube_color);

                // Position and draw the cube
                push();
                translate
                (
                    (x - _cols / 2) * 35,  // X position (centered)
                    y,                     // Y position (height based on wave)
                    (z - _rows / 2) * 35   // Z position (centered)
                );

                if ((x * z) % 2)
                {
                    box(30, 30, 30); // Draw the cube with size 30
                }
                else
                {
                    sphere(15);
                }

                pop();
            }
        }
    }
}