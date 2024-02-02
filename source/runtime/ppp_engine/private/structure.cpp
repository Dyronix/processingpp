#include "structure.h"
#include "device/device.h"

namespace ppp
{
    namespace structure
    {
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
}