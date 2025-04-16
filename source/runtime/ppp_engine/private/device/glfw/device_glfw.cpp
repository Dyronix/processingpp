#include "device/device.h"
#include "device/device_input.h"

#include "util/log.h"
#include "util/types.h"
#include "util/steady_clock.h"

#include "memory/memory_types.h"

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace device
    {
        constexpr s32 _total_avg_frames = 100;

        struct device_ctx
        {
            GLFWwindow* window = nullptr;

            bool is_headless = false;
            bool is_looping = true;

            u64 current_frame_idx = 0;
            u64 desired_frame_idx = 1;

            u32 target_frame_rate = 60;

            f64 frame_times[_total_avg_frames] = { 0.0 };

            clock::time_point previous_frame_time;
            clock::milliseconds delta_frame_time = { clock::milliseconds(0) };

            global_hash_map<u32, bool> key_pressed;
            global_hash_map<u32, bool> key_down;
            global_hash_map<u32, bool> key_released;

            s32 last_key_pressed = -1;

            f32 prev_mouse_x = -1;
            f32 current_mouse_x = -1;
            f32 prev_mouse_y = -1;
            f32 current_mouse_y = -1;

            global_hash_map<u32, bool> mouse_button_pressed;
            global_hash_map<u32, bool> mouse_button_released;

            s32 last_mouse_button_pressed = -1;

            f32 scroll_offset_x = 0.0f;
            f32 scroll_offset_y = 0.0f;

            global_vector<std::function<void(f32, f32)>> dragging_callback;
        };

        static device_ctx& ctx()
        {
            static device_ctx s_ctx;

            return s_ctx;
        }

        namespace internal
        {
            void center_window(GLFWwindow* window) 
            {
                // Get window position and size
                int window_x, window_y;
                glfwGetWindowPos(window, &window_x, &window_y);

                int window_width, window_height;
                glfwGetWindowSize(window, &window_width, &window_height);

                // Halve the window size and use it to adjust the window position to the center of the window
                window_width *= 0.5;
                window_height *= 0.5;

                window_x += window_width;
                window_y += window_height;

                // Get the list of monitors
                int monitors_length;
                GLFWmonitor** monitors = glfwGetMonitors(&monitors_length);

                if (monitors == NULL) {
                    // Got no monitors back
                    return;
                }

                // Figure out which monitor the window is in
                GLFWmonitor* owner = NULL;
                int owner_x, owner_y, owner_width, owner_height;

                for (int i = 0; i < monitors_length; i++) {
                    // Get the monitor position
                    int monitor_x, monitor_y;
                    glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

                    // Get the monitor size from its video mode
                    int monitor_width, monitor_height;
                    GLFWvidmode* monitor_vidmode = (GLFWvidmode*)glfwGetVideoMode(monitors[i]);

                    if (monitor_vidmode == NULL) {
                        // Video mode is required for width and height, so skip this monitor
                        continue;

                    }
                    else {
                        monitor_width = monitor_vidmode->width;
                        monitor_height = monitor_vidmode->height;
                    }

                    // Set the owner to this monitor if the center of the window is within its bounding box
                    if ((window_x > monitor_x && window_x < (monitor_x + monitor_width)) && (window_y > monitor_y && window_y < (monitor_y + monitor_height))) {
                        owner = monitors[i];

                        owner_x = monitor_x;
                        owner_y = monitor_y;

                        owner_width = monitor_width;
                        owner_height = monitor_height;
                    }
                }

                if (owner != NULL) {
                    // Set the window position to the center of the owner monitor
                    glfwSetWindowPos(window, owner_x + (owner_width * 0.5) - window_width, owner_y + (owner_height * 0.5) - window_height);
                }
            }
        }

        namespace input
        {
                namespace callbacks
                {
                    void key_pressed_callback(s32 key, s32 scancode, s32 mods)
                    {
                        ctx().last_key_pressed = key;
                    }
                }

                bool is_key_pressed(s32 code)
                {
                    bool r = is_key_pressed(ctx().window, code);

                    ctx().key_pressed[code] = r;
                    ctx().key_down[code] = false;
                    ctx().key_released[code] = false;

                    return r;
                }

                bool is_key_released(s32 code)
                {
                    bool r = is_key_released(ctx().window, code);

                    ctx().key_pressed[code] = false;
                    ctx().key_down[code] = false;
                    ctx().key_released[code] = r;

                    return r;
                }

                bool is_key_down(s32 code)
                {
                    bool r = is_key_down(ctx().window, code);

                    ctx().key_pressed[code] = false;
                    ctx().key_down[code] = r;
                    ctx().key_released[code] = false;

                    return r;
                }

                bool is_any_key_pressed()
                {
                    return std::any_of(ctx().key_pressed.begin(), ctx().key_pressed.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                bool is_any_key_released()
                {
                    return std::any_of(ctx().key_released.begin(), ctx().key_released.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                bool is_any_key_down()
                {
                    return std::any_of(ctx().key_down.begin(), ctx().key_down.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                s32 key()
                {
                    return ctx().last_key_pressed;
                }

                void add_key_pressed_callback(const std::function<void(s32, s32, s32)>& callback)
                {
                    add_key_pressed_callback(ctx().window, callback);
                }

                void add_key_released_callback(const std::function<void(s32, s32, s32)>& callback)
                {
                    add_key_released_callback(ctx().window, callback);
                }

                void add_key_down_callback(const std::function<void(s32, s32, s32)>& callback)
                {
                    add_key_down_callback(ctx().window, callback);
                }

                namespace callbacks
                {
                    void mouse_pressed_callback(s32 mouse_button, s32 mods)
                    {
                        ctx().last_mouse_button_pressed = mouse_button;
                    }

                    void mouse_moved_callback(f32 xpos, f32 ypos)
                    {
                        if (input::is_any_mouse_button_pressed())
                        {
                            for (const auto& c : ctx().dragging_callback)
                            {
                                c(xpos, ypos);
                            }
                        }
                    }

                    void scroll_x_callback(f32 xoffset)
                    {
                        ctx().scroll_offset_x = xoffset;
                    }

                    void scroll_y_callback(f32 yoffset)
                    {
                        ctx().scroll_offset_y = yoffset;
                    }
                }

                f32 moved_x()
                {
                    if (prev_mouse_x() != -1.0f && mouse_x() != -1.0f)
                    {
                        return mouse_x() - prev_mouse_x();
                    }

                    return 0;
                }

                f32 moved_y()
                {
                    if (prev_mouse_y() != -1.0f && mouse_y() != -1.0f)
                    {
                        return mouse_y() - prev_mouse_y();
                    }

                    return 0;
                }

                f32 mouse_x()
                {
                    return ctx().current_mouse_x;
                }

                f32 mouse_y()
                {
                    return ctx().current_mouse_y;
                }

                f32 prev_mouse_x()
                {
                    return ctx().prev_mouse_x;
                }

                f32 prev_mouse_y()
                {
                    return ctx().prev_mouse_y;
                }

                s32 mouse_button()
                {
                    return ctx().last_mouse_button_pressed;
                }

                f32 scroll_offset_x()
                {
                    return ctx().scroll_offset_x;
                }

                f32 scroll_offset_y()
                {
                    return ctx().scroll_offset_y;
                }

                bool is_left_button_pressed()
                {
                    bool r = is_mouse_button_pressed(ctx().window, GLFW_MOUSE_BUTTON_LEFT);

                    ctx().mouse_button_pressed[GLFW_MOUSE_BUTTON_LEFT] = r;
                    ctx().mouse_button_released[GLFW_MOUSE_BUTTON_LEFT] = false;

                    return r;
                }

                bool is_right_button_pressed()
                {
                    bool r = is_mouse_button_pressed(ctx().window, GLFW_MOUSE_BUTTON_RIGHT);

                    ctx().mouse_button_pressed[GLFW_MOUSE_BUTTON_RIGHT] = r;
                    ctx().mouse_button_released[GLFW_MOUSE_BUTTON_RIGHT] = false;

                    return r;
                }

                bool is_middle_button_pressed()
                {
                    bool r = is_mouse_button_pressed(ctx().window, GLFW_MOUSE_BUTTON_MIDDLE);

                    ctx().mouse_button_pressed[GLFW_MOUSE_BUTTON_MIDDLE] = r;
                    ctx().mouse_button_released[GLFW_MOUSE_BUTTON_MIDDLE] = false;

                    return r;
                }

                bool is_any_mouse_button_pressed()
                {
                    return std::any_of(ctx().mouse_button_pressed.begin(), ctx().mouse_button_pressed.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                bool is_any_mouse_button_released()
                {
                    return std::any_of(ctx().mouse_button_released.begin(), ctx().mouse_button_released.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                void add_mouse_moved_callback(const std::function<void(f32, f32)>& callback)
                {
                    add_mouse_pos_callback(ctx().window, callback);
                }

                void add_mouse_dragged_callback(const std::function<void(f32, f32)>& callback)
                {
                    ctx().dragging_callback.push_back(callback);
                }

                void add_mouse_pressed_callback(const std::function<void(s32, s32)>& callback)
                {
                    add_mouse_button_pressed_callback(ctx().window, callback);
                }

                void add_mouse_released_callback(const std::function<void(s32, s32)>& callback)
                {
                    add_mouse_button_released_callback(ctx().window, callback);
                }

                void add_mouse_horizontal_wheel_callback(const std::function<void(f32)>& callback)
                {
                    add_mouse_scroll_x_callback(ctx().window, callback);
                }

                void add_mouse_vertical_wheel_callback(const std::function<void(f32)>& callback)
                {
                    add_mouse_scroll_y_callback(ctx().window, callback);
                }

                void request_pointer_lock()
                {
                    lock_cursor(ctx().window);
                }

                void request_pointer_hide()
                {
                    hide_cursor(ctx().window);
                }

                void request_pointer_unlock()
                {
                    unlock_cursor(ctx().window);
                }

                void request_pointer_show()
                {
                    show_cursor(ctx().window);
                }
            }

        bool initialize(s32 w, s32 h)
        {
            constexpr s32 required_version_major = 4;
            constexpr s32 required_version_minor = 3;

            // glfw: initialize and configure
            // ------------------------------
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, required_version_major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, required_version_minor);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_DEPTH_BITS, 24);

#ifdef _DEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

            // glfw window creation
            // --------------------
            ctx().window = glfwCreateWindow(w, h, "Processing", NULL, NULL);
            if (ctx().window == NULL)
            {
                // Try and fallback to an earlier version of OpenGL
                constexpr s32 fallback_version_major = 3;
                constexpr s32 fallback_version_minor = 3;

                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, fallback_version_major);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, fallback_version_minor);

                ctx().window = glfwCreateWindow(w, h, "Processing", NULL, NULL);
                if (ctx().window == NULL)
                {
                    log::error("Failed to create GLFW window, minimum required OpenGL version {}.{}", fallback_version_major, fallback_version_minor);

                    terminate();
                    return false;
                }
            }

            glfwMakeContextCurrent(ctx().window);
            glfwSwapInterval(1);

            // Register the key pressed callback so we can cache the last key that was pressed
            input::add_key_pressed_callback(input::callbacks::key_pressed_callback);

            // Register the mouse button pressed callback so we can cache the last key that was pressed
            input::add_mouse_pressed_callback(input::callbacks::mouse_pressed_callback);

            // Register the mouse move callback so we can check if we are dragging along the screen
            input::add_mouse_moved_callback(input::callbacks::mouse_moved_callback);

            // Register the mouse scroll callback so we can check if we are scrolling
            input::add_mouse_horizontal_wheel_callback(input::callbacks::scroll_x_callback);
            input::add_mouse_vertical_wheel_callback(input::callbacks::scroll_y_callback);

            internal::center_window(ctx().window);

            // Initialize frame time
            ctx().previous_frame_time = clock::now();

            return true;
        }
        void terminate()
        {
            // glfw: terminate, clearing all previously allocated GLFW resources.
            // ------------------------------------------------------------------
            glfwTerminate();
        }

        void tick()
        {
            ctx().prev_mouse_x = ctx().current_mouse_x;
            ctx().prev_mouse_y = ctx().current_mouse_y;
            ctx().current_mouse_x = input::mouse_x(ctx().window);
            ctx().current_mouse_y = input::mouse_y(ctx().window);

            ctx().scroll_offset_x = 0.0f;
            ctx().scroll_offset_y = 0.0f;

            auto current_frame_time = clock::now();

            ctx().delta_frame_time = clock::duration(ctx().previous_frame_time, current_frame_time);
            ctx().previous_frame_time = current_frame_time;

            if (ctx().is_looping)
            {
                ctx().frame_times[current_frame_index() % _total_avg_frames] = delta_time();
            }
        }

        void window_width(s32* w)
        {
            s32 height = 0;
            glfwGetWindowSize(ctx().window, w, &height);
        }
        void window_height(s32* h)
        {
            s32 width = 0;
            glfwGetWindowSize(ctx().window, &width, h);
        }

        void headless()
        {
            ctx().is_headless = true;
        }

        void loop()
        {
            ctx().is_looping = true;
        }

        void no_loop()
        {
            ctx().is_looping = false;
        }

        void redraw()
        {
            --ctx().current_frame_idx;
        }

        void present()
        {
            glfwSwapBuffers(ctx().window);

            // Make sure we don't overflow
            // Reset the frame indices to their initial value if we reach a certain threshold
            if (ctx().desired_frame_idx + 1 > std::numeric_limits<u32>::max())
            {
                ctx().current_frame_idx = 0;
                ctx().desired_frame_idx = 1;
            }

            ctx().current_frame_idx = ctx().desired_frame_idx;

            if (is_looping())
            {
                ++ctx().desired_frame_idx;
            }
        }

        void poll_events()
        {
            glfwPollEvents();
        }

        void request_quit()
        {
            glfwSetWindowShouldClose(ctx().window, true);
        }

        void target_frame_rate(u32 rate)
        {
            ctx().target_frame_rate = rate;
        }

        bool is_headless()
        {
            return ctx().is_headless;
        }

        bool is_looping()
        {
            return ctx().is_looping;
        }

        bool can_draw()
        {
            return is_looping() || ctx().current_frame_idx < ctx().desired_frame_idx;
        }

        bool should_close()
        {
            return glfwWindowShouldClose(ctx().window);
        }

        u32 current_frame_index()
        {
            return ctx().current_frame_idx;
        }

        u32 desired_frame_index()
        {
            return ctx().desired_frame_idx;
        }

        u32 current_frame_rate()
        {
            if (ctx().is_looping)
            {
                return 1.0f / delta_time();
            }

            return 0.0f;
        }

        u32 average_frame_rate()
        {
            f64 total_frame_time = 0.0;
            for (s32 i = 0; i < _total_avg_frames; ++i)
            {
                total_frame_time += ctx().frame_times[i];
            }

            if (total_frame_time > 0.0) 
            {
                auto fps = _total_avg_frames / total_frame_time;
                return fps;
            }

            return 0;
        }

        u32 target_frame_rate()
        {
            return ctx().target_frame_rate;
        }

        f32 max_frame_time()
        {
            return 1.0f / ctx().target_frame_rate;
        }

        f32 delta_time()
        {
            if (ctx().is_looping)
            {
                return std::chrono::duration<float>(ctx().delta_frame_time).count();
            }
            
            log::info("We specified that the app should not be looping, calling delta_time could result in weird results. returning 0");
            return 0.0f;
        }

        f32 total_time()
        {
            return glfwGetTime();
        }
    }
}