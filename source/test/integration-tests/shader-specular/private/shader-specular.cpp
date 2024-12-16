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
#include "material.h"
#include "image.h"

namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _no_testing = false;

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    constexpr int _total_shape_count = 8;

    int _shape_vis = 0;
    int _show_all = 1;

    int _interpolation = 8;

    float _specular_strength = 0.5f;
    int _specular_shininess = 32;

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);

        keyboard::add_key_pressed_callback(
            [](keyboard::KeyCode key)
        {
            if (key == keyboard::KeyCode::KEY_SPACE)
            {
                bool show_all_shapes = _show_all > 0;
                show_all_shapes = !show_all_shapes;
                _show_all = show_all_shapes ? 1 : 0;
            }

            else if (key == keyboard::KeyCode::KEY_UP && _show_all == 0)
            {
                _shape_vis = (_shape_vis + 1) % _total_shape_count;
            }
            else if (key == keyboard::KeyCode::KEY_DOWN && _show_all == 0)
            {
                _shape_vis = (_shape_vis - 1) < 0 ? _total_shape_count - 1 : _shape_vis - 1;
            }

            else if (key == keyboard::KeyCode::KEY_1)
            {
                _interpolation = 4;
                environment::print("_interpolation: " + std::to_string(_interpolation));
            }
            else if (key == keyboard::KeyCode::KEY_2)
            {
                _interpolation = 8;
                environment::print("_interpolation: " + std::to_string(_interpolation));
            }
            else if (key == keyboard::KeyCode::KEY_3)
            {
                _interpolation = 12;
                environment::print("_interpolation: " + std::to_string(_interpolation));
            }
            else if (key == keyboard::KeyCode::KEY_4)
            {
                _interpolation = 24;
                environment::print("_interpolation: " + std::to_string(_interpolation));
            }

            else if (key == keyboard::KeyCode::KEY_W)
            {
                _specular_shininess = _specular_shininess >> 1;
                if (_specular_shininess < 2)
                {
                    _specular_shininess = 2;
                }
                environment::print("_specular_shininess: " + std::to_string(_specular_shininess));
            }
            else if (key == keyboard::KeyCode::KEY_A)
            {
                _specular_strength = _specular_strength += 0.1f;
                if (_specular_strength > 1.0f)
                {
                    _specular_strength = 1.0f;
                }
                environment::print("_specular_strength: " + std::to_string(_specular_strength));
            }
            else if (key == keyboard::KeyCode::KEY_S)
            {
                _specular_shininess = _specular_shininess << 1;
                if (_specular_shininess > 256)
                {
                    _specular_shininess = 256;
                }
                environment::print("_specular_shininess: " + std::to_string(_specular_shininess));
            }
            else if (key == keyboard::KeyCode::KEY_D)
            {
                _specular_strength = _specular_strength -= 0.1f;
                if (_specular_strength < 0.0f)
                {
                    _specular_strength = 0.0f;
                }
                environment::print("_specular_strength: " + std::to_string(_specular_strength));
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
                image::save_pixels("local:/test-shader-specular.png", _window_width, _window_height);
            }

            if (!_no_testing)
            {
                auto test_frame = image::load("local:/test-shader-specular.png");
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
                    environment::print("[TEST FAILED][SSPEC] image buffers are not identical!");
                }
                else
                {
                    environment::print("[TEST SUCCESS][SSPEC] image buffers are identical.");
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
        environment::print("current working dir: " + environment::cwd());

        app_params app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        _generate_new_data = has_argument(argc, argv, "--generate-new-data");
        _no_close_after_x_frames = has_argument(argc, argv, "--no_close");
        _no_testing = has_argument(argc, argv, "--no_testing");

        return app_params;
    }

    void setup()
    {
        setup_input_events();

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::set_scene_camera(20, -40, 400);

        material::specular_material();

        structure::on_draw_end(end_draw);
    }

    void draw_shapes_grid()
    {
        if (_show_all)
        {
            float start_x = -120.0f; // Initial x position to start grid from
            float start_y = 40.0f;    // Initial y position for the grid row
            float x_spacing = 80.0f; // Horizontal spacing between shapes
            float y_spacing = -80.0f; // Vertical spacing between rows

            transform::push();

            // Row 1
            transform::translate(start_x, start_y);
            shapes::box(50.0f, 50.0f, 50.0f);
            transform::translate(x_spacing, 0.0f);
            shapes::plane(50.0f, 50.0f);
            transform::translate(x_spacing, 0.0f);
            shapes::cylinder(25.0f, 50.0f, _interpolation);
            transform::translate(x_spacing, 0.0f);
            shapes::sphere(25.0f, _interpolation);

            // Move to next row and reset x position
            transform::translate(-3 * x_spacing, y_spacing);

            // Row 2
            shapes::torus(25.0f, 10.0f, _interpolation, _interpolation);
            transform::translate(x_spacing, 0.0f);
            shapes::cone(25.0f, 50.0f, _interpolation, true);
            transform::translate(x_spacing, 0.0f);
            shapes::tetrahedron(25.0f, 25.0f);
            transform::translate(x_spacing, 0.0f);
            shapes::octahedron(25.0f, 25.0f);

            transform::pop();
        }
        else
        {
            float start_x = -120.0f; // Initial x position to start grid from
            float start_y = 40.0f;    // Initial y position for the grid row
            float x_spacing = 80.0f; // Horizontal spacing between shapes
            float y_spacing = -80.0f; // Vertical spacing between rows

            transform::push();

            // Row 1
            transform::translate(start_x, start_y);
            if (_shape_vis == 0) { shapes::box(50.0f, 50.0f, 50.0f); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 1) { shapes::plane(50.0f, 50.0f); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 2) { shapes::cylinder(25.0f, 50.0f, _interpolation); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 3) { shapes::sphere(25.0f, _interpolation); }

            // Move to next row and reset x position
            transform::translate(-3 * x_spacing, y_spacing);

            // Row 2
            if (_shape_vis == 4) { shapes::torus(25.0f, 10.0f, _interpolation, _interpolation); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 5) { shapes::cone(25.0f, 50.0f, _interpolation, true); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 6) { shapes::tetrahedron(25.0f, 25.0f); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 7) { shapes::octahedron(25.0f, 25.0f); }

            transform::pop();
        }
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

        auto specular_program = material::get_shader("lit_specular");

        specular_program.set_uniform("u_ambient_strength", 0.1f);
        specular_program.set_uniform("u_light_position", glm::vec3{ 0.0f, 0.0f, 200.0f });
        specular_program.set_uniform("u_light_color", glm::vec3{ 1.0f, 1.0f, 1.0f });
        specular_program.set_uniform("u_specular_color", glm::vec3{ 1.0f, 1.0f, 1.0f });
        specular_program.set_uniform("u_specular_strength", _specular_strength);
        specular_program.set_uniform("u_specular_power", _specular_shininess);
        specular_program.set_uniform("u_view_position", camera::active_camera_position());

        color::fill({ 255,0,0,255 });

        draw_shapes_grid();

        color::fill({0,0,0,255});
    }
}