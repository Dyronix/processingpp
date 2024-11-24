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

#define PPP_CHECK_TEST_FRAME 1
#define PPP_SAVE_TEST_FRAME 0
#define PPP_CLOSE_AFTER_X_FRAMES 1

// Demo of shapes and images with stroke and inner stroke
namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    image::image _image_container;
    image::image _image_wall;

    void setup_canvas()
    {
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

    void end_draw()
    {
        if (environment::frame_count() == 5)
        {
#if PPP_SAVE_TEST_FRAME
            image::load_pixels(0, 0, _window_width, _window_height);
            image::save_pixels("local:/test-geometry-2d.png", _window_width, _window_height);
#endif

#if PPP_CHECK_TEST_FRAME
            auto test_frame = image::load("local:/test-geometry-2d.png");
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
                environment::print("[TEST FAILED][G2D] image buffers are not identical!");
            }
            else
            {
                environment::print("[TEST SUCCESS][G2D] image buffers are identical.");
            }
#endif

#if PPP_CLOSE_AFTER_X_FRAMES
            structure::quit();
#endif
        }
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

        shapes::rect_mode(shapes::shape_mode_type::CENTER);
        shapes::ellipse_mode(shapes::shape_mode_type::CENTER);

        image::image_mode(image::image_mode_type::CORNER);

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        _image_container = image::load("local:content/container.jpg");
        _image_wall = image::load("local:content/wall.jpg");

        structure::on_draw_end(end_draw);
    }

    void draw()
    {
        color::fill({ 255,0,0,255 });

        color::stroke({ 255, 255 , 255, 255 });
        color::stroke_weight(5.0f);
        color::inner_stroke({ 0, 255 , 0, 255 });
        color::inner_stroke_weight(5.0f);

        int shape_count = 8;
        int shape_size = 50;

        int x = _window_width * 0.5f - ((shape_count - 1) * shape_size);
        int y = _window_height * 0.5f;

        shapes::ellipse(x, y, shape_size * 0.5f, shape_size * 0.5f);
        
        x += shape_size * 2;
        
        shapes::circle(x, y, shape_size * 0.5f, 4);
        
        // circles are drawn from the center so we subtract have the radius here
        x += (shape_size * 2) - shape_size * 0.5f;
        
        shapes::line(x, y, x + shape_size, y);
        
        x += shape_size * 2;
        
        shapes::point(x, y);
        
        x += shape_size * 2;
        
        shapes::polygon(x - (shape_size * 0.5f), y - (shape_size * 0.5f), x + (shape_size * 0.5f), y - (shape_size * 0.5f), x + (shape_size * 0.5f), y + (shape_size * 0.5f), x - (shape_size * 0.5f), y + (shape_size * 0.5f));
        
        x += shape_size * 2;
        
        shapes::rect(x, y, shape_size, shape_size);
        
        x += shape_size * 2;
        
        shapes::square(x, y, shape_size);
        
        x += shape_size * 2;
        
        shapes::triangle(x + shape_size, y - (shape_size * 0.5f), x + (shape_size * 0.5f), y + (shape_size * 0.5f), x, y - (shape_size * 0.5f));
        
        x = 96;
        for (int i = 0; i < 9; ++i)
        {
            if (i % 2)
            {
                image::draw(_image_container.id, x, 64.0f, 64, 64);
            }
            else
            {
                image::draw(_image_wall.id, x, 64.0f, 64, 64);
            }
            x += 128;
        }     
    }
}