#include "environment.h"
#include "device/device.h"
#include "render/render.h"
#include "string/string_id.h"
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
            string::string_id& cwd()
            {
                static string::string_id s_cwd;

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
            if (internal::cwd().is_none())
            {
                temp_string s_cwd = std::filesystem::current_path().string<char, std::char_traits<char>, memory::frame_heap_allocator<char>>();

                internal::cwd() = string::store_sid(s_cwd);
            }

            return string::restore_sid(internal::cwd());
        }
    }
}