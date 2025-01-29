#include "engine.h"

#include <sstream>

namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _no_testing = false;

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
                    environment::print("[TEST FAILED][G2D] image buffers are not identical!");
                }
                else
                {
                    environment::print("[TEST SUCCESS][G2D] image buffers are identical.");
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

        app_params.window_width = 1280;
        app_params.window_height = 720;

        _generate_new_data = has_argument(argc, argv, "--generate-new-data");
        _no_close_after_x_frames = has_argument(argc, argv, "--no_close");
        _no_testing = has_argument(argc, argv, "--no_testing");

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

        _image_container = image::load("local:content/t_container.jpg");
        _image_wall = image::load("local:content/t_wall.jpg");

        structure::on_draw_end(end_draw);
        
        camera::set_scene_camera_mode(camera::scene_camera_mode::CAMERA_IMAGE);
    }

    void draw()
    {
        color::fill({ 255,0,0,255 });

        color::stroke({ 255, 255 , 255, 255 });
        color::stroke_weight(5.0f);
        color::inner_stroke({ 0, 255 , 0, 255 });
        color::inner_stroke_weight(5.0f);

        material::shader(material::tags::unlit_color());

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
        
        material::shader(material::tags::unlit_texture());

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