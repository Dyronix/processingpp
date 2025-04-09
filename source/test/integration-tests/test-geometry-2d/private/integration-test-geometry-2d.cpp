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

    image _image_container;
    image _image_wall;

    void setup_canvas()
    {
        background(15);
    }

    void setup_input_events()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);
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
                    print("[TEST FAILED][G2D] image buffers are not identical!");
                }
                else
                {
                    print("[TEST SUCCESS][G2D] image buffers are identical.");
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

        app_params.window_width = 1280;
        app_params.window_height = 720;

        _generate_new_data = has_argument(argc, argv, "--generate-new-data");
        _no_close_after_x_frames = has_argument(argc, argv, "--no-close");
        _no_testing = has_argument(argc, argv, "--no-testing");

        return app_params;
    }

    void setup()
    {
        setup_canvas();
        setup_input_events();

        rect_mode(shape_mode_type::CENTER);
        ellipse_mode(shape_mode_type::CENTER);

        image_mode(image_mode_type::CORNER);

        enable_wireframe_mode(false);
        enable_solid_mode(true);

        _image_container = load("local:content/t_container.jpg");
        _image_wall = load("local:content/t_wall.jpg");

        on_draw_end(end_draw);      

        activate_camera(camera::tags::orthographic());
    }

    void draw()
    {
        fill({ 255,0,0,255 });

        stroke({ 255, 255 , 255, 255 });
        stroke_weight(5.0f);
        inner_stroke({ 0, 255 , 0, 255 });
        inner_stroke_weight(5.0f);

        shader(material::tags::unlit::color());

        int shape_count = 8;
        int shape_size = 50;

        int x = _window_width * 0.5f - ((shape_count - 1) * shape_size);
        int y = _window_height * 0.5f;

        ellipse(x, y, shape_size * 0.5f, shape_size * 0.5f);
        
        x += shape_size * 2;
        
        circle(x, y, shape_size * 0.5f, 4);
        
        // circles are drawn from the center so we subtract have the radius here
        x += (shape_size * 2) - shape_size * 0.5f;
        
        line(x, y, x + shape_size, y);
        
        x += shape_size * 2;
        
        point(x, y);
        
        x += shape_size * 2;
        
        polygon(x - (shape_size * 0.5f), y - (shape_size * 0.5f), x + (shape_size * 0.5f), y - (shape_size * 0.5f), x + (shape_size * 0.5f), y + (shape_size * 0.5f), x - (shape_size * 0.5f), y + (shape_size * 0.5f));
        
        x += shape_size * 2;
        
        rect(x, y, shape_size, shape_size);
        
        x += shape_size * 2;
        
        square(x, y, shape_size);
        
        x += shape_size * 2;
        
        triangle(x + shape_size, y - (shape_size * 0.5f), x + (shape_size * 0.5f), y + (shape_size * 0.5f), x, y - (shape_size * 0.5f));
        
        shader(material::tags::unlit::texture());

        x = 96;
        for (int i = 0; i < 9; ++i)
        {
            if (i % 2)
            {
                draw(_image_container.id, x, 64.0f, 64, 64);
            }
            else
            {
                draw(_image_wall.id, x, 64.0f, 64, 64);
            }
            x += 128;
        }     
    }
}