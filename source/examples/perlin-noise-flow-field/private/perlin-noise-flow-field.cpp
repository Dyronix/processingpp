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
        image::image_id _flow_field;
        
        std::array<unsigned char, 600 * 600 * 4> _flow_field_pixels;
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

        internal::_flow_field = image::create(600, 600, 4, nullptr);
        std::fill(internal::_flow_field_pixels.begin(), internal::_flow_field_pixels.end(), 0xFF);
    }

    void draw()
    {
        image::update(internal::_flow_field, 0, 0, 600, 600, 4, internal::_flow_field_pixels.data());
        image::draw(internal::_flow_field, 100, 0, 600, 600);
    }
}