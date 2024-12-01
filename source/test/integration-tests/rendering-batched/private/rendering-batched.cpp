#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "typography.h"
#include "environment.h"
#include "camera.h"
#include "mathematics.h"
#include "image.h"
#include "rendering.h"

#define PPP_CHECK_TEST_FRAME 1
#define PPP_SAVE_TEST_FRAME 0
#define PPP_CLOSE_AFTER_X_FRAMES 1
#define PPP_ANIMATE_SCENE 0

namespace ppp
{
    bool _generate_new_data = false;

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

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
    color::Color _color_top;
    color::Color _color_bottom;

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);

        keyboard::add_key_pressed_callback(
            [](keyboard::KeyCode key)
        {
            if (key == keyboard::KeyCode::KEY_UP)
            {
                _rows += 1;
            }
            else if (key == keyboard::KeyCode::KEY_DOWN)
            {
                _rows -= 1;
            }

            if (key == keyboard::KeyCode::KEY_RIGHT)
            {
                _cols += 1;
            }
            else if (key == keyboard::KeyCode::KEY_LEFT)
            {
                _cols -= 1;
            }
        });
    }

    void end_draw()
    {
        if (environment::frame_count() == 5)
        {
            if (_generate_new_data)
            {
                image::load_pixels(0, 0, _window_width, _window_height);
                image::save_pixels("local:/test-rendering-batched.png", _window_width, _window_height);
            }

            #if PPP_CHECK_TEST_FRAME
            auto test_frame = image::load("local:/test-rendering-batched.png");
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
                environment::print("[TEST FAILED][RBATCH] image buffers are not identical!");
            }
            else
            {
                environment::print("[TEST SUCCESS][RBATCH] image buffers are identical.");
            }
            #endif

            #if PPP_CLOSE_AFTER_X_FRAMES
            structure::quit();
            #endif
        }
    }

    AppParams entry(int argc, char** argv)
    {
        environment::print("current working dir: " + environment::cwd());

        AppParams app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        _generate_new_data = find_argument(argc, argv, "--generate-new-data");

        return app_params;
    }

    void setup()
    {
        setup_input_events();

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 1000.0f);
        camera::camera(20, -40, 600);

        _color_top = { 255, 0, 0, 255 };     // Red for top
        _color_bottom = { 0, 0, 255, 255 };  // Blue for bottom

        structure::on_draw_end(end_draw);

        rendering::enable_batched_draw_mode();
    }

    void draw()
    {
        color::background(200);

        camera::OrbitCameraOptions options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        camera::orbit_control(options);

        // Calculate wave offsets based on frame count
        #if PPP_ANIMATE_SCENE
        float wave_offset_x = environment::frame_count() * _movement_speed_x;
        float wave_offset_z = environment::frame_count() * _movement_speed_z;
        #else
        float wave_offset_x = 0.0f;
        float wave_offset_z = 0.0f;
        #endif

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
                color::Color cube_color = color::lerp_color(_color_bottom, _color_top, color_interp);
                color::fill(cube_color);

                // Position and draw the cube
                transform::push();
                transform::translate
                (
                    (x - _cols / 2) * 35,  // X position (centered)
                    y,                     // Y position (height based on wave)
                    (z - _rows / 2) * 35   // Z position (centered)
                );

                if ((x * z) % 2)
                {
                    shapes::box(30, 30, 30); // Draw the cube with size 30
                }
                else
                {
                    shapes::sphere(15);
                }

                transform::pop();
            }
        }
    }
}