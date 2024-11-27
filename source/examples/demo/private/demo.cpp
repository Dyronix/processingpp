#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"
#include "image.h"
#include "typography.h"
#include "camera.h"
#include "material.h"

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;
    
    int _interpolation = 8;

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);

        mouse::add_mouse_pressed_callback(
            [](mouse::mouse_code code)
        {
            if (code == mouse::mouse_code::BUTTON_LEFT)
            {
                structure::redraw();
            }
        });
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

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::camera(20, -40, 400);

        rendering::enable_instance_draw_mode();
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

        color::fill({ 255,0,0,255 });

        float start_x = -120.0f; // Initial x position to start grid from
        float start_y = 40.0f;    // Initial y position for the grid row
        float x_spacing = 80.0f; // Horizontal spacing between shapes
        float y_spacing = -80.0f; // Vertical spacing between rows

        transform::push();

        // Row 1
        transform::translate(start_x, start_y);
        shapes::box(50.0f, 50.0f, 50.0f);
        //transform::translate(x_spacing, 0.0f);
        //shapes::plane(50.0f, 50.0f);
        //transform::translate(x_spacing, 0.0f);
        //shapes::cylinder(25.0f, 50.0f, _interpolation);
        //transform::translate(x_spacing, 0.0f);
        //shapes::sphere(25.0f, _interpolation);

        //// Move to next row and reset x position
        //transform::translate(-3 * x_spacing, y_spacing);

        //// Row 2
        //shapes::torus(25.0f, 10.0f, _interpolation, _interpolation);
        //transform::translate(x_spacing, 0.0f);
        //shapes::cone(25.0f, 50.0f, _interpolation, true);
        //transform::translate(x_spacing, 0.0f);
        //shapes::tetrahedron(25.0f, 25.0f);
        //transform::translate(x_spacing, 0.0f);
        //shapes::octahedron(25.0f, 25.0f);

        transform::pop();

        color::fill({ 0,0,0,255 });
    }
}