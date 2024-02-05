#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"
#include "image.h"

namespace ppp
{
    namespace internal
    {
        constexpr int image_width = 600;
        constexpr int image_height = 600;
        constexpr int image_channels = 4;

        image::image_id _flow_field;
        
        std::array<unsigned int, image_width * image_height> _flow_field_pixels;
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
        keyboard::add_key_pressed_callback(
            [](keyboard::KeyCode key)
        {
            if (key == keyboard::KeyCode::KEY_ESCAPE)
            {
                structure::quit();
            }
        });

        rendering::create_canvas((1280 / 2) - 300, (720 / 2) - 300, 600.0f, 600.0f);

        color::background(15);

        for (int y = 0; y < internal::image_height; ++y)
        {
            for (int x = 0; x < internal::image_width; ++x)
            {
                int r = rand() % 256;

                int index = y * internal::image_width + x;

                int color = 0;
                color  = 255 << 24;
                color |= r << 16;
                color |= r << 8;
                color |= r << 0;

                internal::_flow_field_pixels[index] = color;
            }
        }

        internal::_flow_field = image::create(internal::image_width, internal::image_height, internal::image_channels, nullptr);
    }

    void draw()
    {
        image::draw(internal::_flow_field, 0, 0, internal::image_width, internal::image_height);
    }
}