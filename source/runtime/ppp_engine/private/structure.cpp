#include "structure.h"
#include "render/helpers/render_event_dispatcher.h"
#include "device/device.h"

namespace ppp
{
    void on_draw_begin(std::function<void()> draw_begin)
    {
        render::register_on_draw_begin(draw_begin);
    }

    void on_draw_end(std::function<void()> draw_end)
    {
        render::register_on_draw_end(draw_end);
    }

    void redraw()
    {
        device::redraw();
    }

    void loop()
    {
        device::loop();
    }

    void no_loop()
    {
        device::no_loop();
    }

    bool is_looping()
    {
        return device::is_looping();
    }

    void quit()
    {
        device::request_quit();
    }
}