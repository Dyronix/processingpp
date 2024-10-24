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

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    typography::font_id _font;
    
    image::image _image_container;
    image::image _image_wall;

    void setup_canvas()
    {
        //rendering::create_canvas((_window_width / 2) - (_canvas_width / 2), (_window_height / 2) - (_canvas_height / 2), _canvas_width, _canvas_height);

        color::background(15);
    }

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

        app_params.window_width = 1280;
        app_params.window_height = 720;

        return app_params;
    }

    void setup()
    {
        setup_canvas();
        setup_input_events();

        load_font();
        activate_font();

        _image_container = image::load("local:content/container.jpg");
        _image_wall = image::load("local:content/wall.jpg");

        shapes::rect_mode(shapes::shape_mode_type::CORNER);
        image::image_mode(image::image_mode_type::CORNER);

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        //camera::perspective(55.0f, _window_width/_window_height, 0.1f, 1000.0f);
        //camera::camera(20, -40, 80);
    }

    void draw()
    {
        camera::OrbitCameraOptions options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;

        //camera::orbit_control(options);

        color::fill({ 255,0,0,255 });
        
        color::stroke({ 255,0,0,255 });
        color::stroke_weight(5.0f);
        
        //shapes::line(10.0f, 100.0f, 400.0f, 100.0f);
        //shapes::line(10.0f, 200.0f, 400.0f, 200.0f);
        //shapes::line(10.0f, 300.0f, 400.0f, 300.0f);
        
        int x = 10;
        
        for (int i = 0; i < 20; ++i)
        {
            shapes::point((float)x, 150.0f);
            shapes::point((float)x, 250.0f);
            shapes::point((float)x, 350.0f);
        
            x += 15;
        }

        //int x = 0;
        //
        //for (int i = -5; i <= 5; ++i)
        //{
        //    transform::push();
        //    transform::translate(x, 0.0f);
        //    if (i % 2)
        //    {
        //        shapes::box(10.0f, 10.0f, 10.0f);
        //        //shapes::cylinder(5.0f, 10.0f, 12.0f);
        //        //shapes::plane(5.0f, 5.0f);
        //        //shapes::torus(6.0f, 2.0f, 8.0f, 8.0f);
        //        //shapes::sphere(6.0f, 12.0f);
        //        //shapes::cone(6.0f, 8.0f);
        //        //shapes::tetrahedron(6.0f, 6.0f);
        //        //shapes::octahedron(6.0f, 6.0f);
        //    }
        //    else
        //    {
        //        //shapes::box(10.0f, 10.0f, 10.0f);
        //        //shapes::cylinder(5.0f, 10.0f, 12.0f);
        //        //shapes::plane(5.0f, 5.0f);
        //        shapes::torus(6.0f, 2.0f, 8.0f, 8.0f);
        //        //shapes::sphere(6.0f, 12.0f);
        //        //shapes::cone(6.0f, 8.0f);
        //        //shapes::tetrahedron(6.0f, 6.0f);
        //        //shapes::octahedron(6.0f, 6.0f);
        //    }
        //    transform::pop();
        //    x = (i * 15);
        //}

        //x = 96;
        //for (int i = 0; i < 9; ++i)
        //{
        //    if (i % 2)
        //    {
        //        image::draw(_image_container.id, x, 64.0f, 64, 64);
        //    }
        //    else
        //    {
        //        image::draw(_image_wall.id, x, 64.0f, 64, 64);
        //    }
        //    x += 128;
        //}

        //std::string str_frame_rate = "fps " + std::to_string(environment::frame_rate());
        //std::string str_delta_time = "ms " + std::to_string(environment::delta_time());

        //typography::text(str_frame_rate, 10, _window_height - 30);
        //typography::text("-", 135, _window_height - 30);
        //typography::text(str_delta_time, 170, _window_height - 30);
    }
}