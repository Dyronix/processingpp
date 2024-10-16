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

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    typography::font_id _font;

    constexpr int _cols = 20; // number of columns in the grid (x-direction)
    constexpr int _rows = 20; // number of rows in the grid (z-direction)

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
    }

    void load_font()
    {
        _font = typography::load_font("local:content/fonts/PokemonGb-RAeo.ttf", 18);
    }

    void activate_font()
    {
        typography::text_font(_font);
    }

    AppParams entry()
    {
        AppParams app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        return app_params;
    }

    void setup()
    {
        setup_input_events();

        load_font();
        activate_font();

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 1000.0f);
        camera::camera(20, -40, 600);

        _color_top = { 255, 0, 0, 255 };     // Red for top
        _color_bottom = { 0, 0, 255, 255 };  // Blue for bottom
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
        float wave_offset_x = environment::frame_count() * _movement_speed_x;
        float wave_offset_z = environment::frame_count() * _movement_speed_z;

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
                shapes::box(30, 30, 30); // Draw the cube with size 30
                transform::pop();
            }
        }

        color::fill({0,0,0,255});

        std::string str_frame_rate = "fps " + std::to_string(environment::frame_rate());
        std::string str_delta_time = "ms " + std::to_string(environment::delta_time());

        typography::text(str_frame_rate, 10, _window_height - 30);
        typography::text("-", 135, _window_height - 30);
        typography::text(str_delta_time, 170, _window_height - 30);
    }
}