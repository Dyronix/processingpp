#include "environment.h"
#include "device/device.h"
#include "render/render.h"
#include "memory/memory_types.h"
#include "memory/memory_tracker.h"
#include "util/log.h"
#include "util/types.h"
#include <filesystem>

namespace ppp
{
    namespace environment
    {
        namespace internal
        {
            pool_string& cwd()
            {
                static pool_string s_cwd;

                return s_cwd;
            }
        }

        void print(std::string_view message)
        {
            log::info(message);
        }

        void print(const char* message)
        {
            log::info(message);
        }

        void frame_rate(unsigned int frame_rate)
        {
            device::target_frame_rate(frame_rate);
        }

        unsigned int average_frame_rate()
        {
            return device::average_frame_rate();
        }

        unsigned int frame_rate()
        {
            return device::current_frame_rate();
        }

        unsigned int frame_count()
        {
            return device::current_frame_index();
        }

        float delta_time()
        {
            return device::delta_time();
        }

        float total_time()
        {
            return device::total_time();
        }

        float window_width()
        {
            s32 width = 0;
            device::window_width(&width);
            return static_cast<f32>(width);
        }

        float window_height()
        {
            s32 height = 0;
            device::window_height(&height);
            return static_cast<f32>(height);
        }

        float canvas_width()
        {
            if (render::scissor_enabled())
            {
                return render::scissor().w;
            }

            return window_width();
        }

        float canvas_height()
        {
            if (render::scissor_enabled())
            {
                return render::scissor().h;
            }

            return window_height();
        }

        std::string_view cwd()
        {
            if (internal::cwd().empty())
            {
                /*
                Unlike some standard containers (like std::basic_string or std::vector), std::filesystem::path does not expose an interface that allows you to provide your own allocator.
                You simply construct it from a std::string or other string types, and the conversion to the internal representation is handled internally.

                If you have a particularly long path or if the implementation doesn’t use SSO, dynamic allocation might be used under the hood.
                There is no way to avoid the allocation and deallocation here unless we write our own wrapper.

                For this reason we disable the tracking here.
                */

                memory::disable_tracking();

                internal::cwd() = std::filesystem::current_path().string<char, std::char_traits<char>, memory::string_heap_allocator<char>>();

                memory::enable_tracking();
            }

            return internal::cwd();
        }
    }
}