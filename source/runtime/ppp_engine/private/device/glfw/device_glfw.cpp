#include "device/device.h"
#include "device/device_input.h"

#include "util/log.h"
#include "util/types.h"
#include "util/steady_clock.h"

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace device
    {
        namespace internal
        {
            GLFWwindow* _window = nullptr;

            bool _is_looping = true;

            u64 _current_frame_idx = 0;
            u64 _desired_frame_idx = 1;

            u32 _target_frame_rate = 60;

            constexpr s32 _total_avg_frames = 100;
            f64 _frame_times[_total_avg_frames] = { 0.0 };

            clock::time_point _previous_frame_time;
            clock::milliseconds _delta_frame_time(0);

            std::unordered_map<u32, bool> _key_pressed;
            std::unordered_map<u32, bool> _key_down;
            std::unordered_map<u32, bool> _key_released;

            s32 _last_key_pressed = -1;

            f32 _prev_mouse_x = -1;
            f32 _current_mouse_x = -1;
            f32 _prev_mouse_y = -1;
            f32 _current_mouse_y = -1;

            std::unordered_map<u32, bool> _mouse_button_pressed;
            std::unordered_map<u32, bool> _mouse_button_released;

            s32 _last_mouse_button_pressed = -1;

            f32 _scroll_offset_x = 0.0f;
            f32 _scroll_offset_y = 0.0f;

            std::vector<std::function<void(f32, f32)>> _dragging_callback;

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
            namespace keyboard
            {
                bool is_key_pressed(s32 code)
                {
                    bool r = is_key_pressed(internal::_window, code);

                    internal::_key_pressed[code] = r;
                    internal::_key_down[code] = false;
                    internal::_key_released[code] = false;

                    return r;
                }

                bool is_key_released(s32 code)
                {
                    bool r = is_key_released(internal::_window, code);

                    internal::_key_pressed[code] = false;
                    internal::_key_down[code] = false;
                    internal::_key_released[code] = r;

                    return r;
                }

                bool is_key_down(s32 code)
                {
                    bool r = is_key_down(internal::_window, code);

                    internal::_key_pressed[code] = false;
                    internal::_key_down[code] = r;
                    internal::_key_released[code] = false;

                    return r;
                }

                bool is_any_key_pressed()
                {
                    return std::any_of(internal::_key_pressed.begin(), internal::_key_pressed.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                bool is_any_key_released()
                {
                    return std::any_of(internal::_key_released.begin(), internal::_key_released.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                bool is_any_key_down()
                {
                    return std::any_of(internal::_key_down.begin(), internal::_key_down.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                s32 key()
                {
                    return internal::_last_key_pressed;
                }

                void add_key_pressed_callback(const std::function<void(s32, s32, s32)>& callback)
                {
                    add_key_pressed_callback(internal::_window, callback);
                }

                void add_key_released_callback(const std::function<void(s32, s32, s32)>& callback)
                {
                    add_key_released_callback(internal::_window, callback);
                }

                void add_key_down_callback(const std::function<void(s32, s32, s32)>& callback)
                {
                    add_key_down_callback(internal::_window, callback);
                }
            }

            namespace mouse
            {
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
                    return internal::_current_mouse_x;
                }

                f32 mouse_y()
                {
                    return internal::_current_mouse_y;
                }

                f32 prev_mouse_x()
                {
                    return internal::_prev_mouse_x;
                }

                f32 prev_mouse_y()
                {
                    return internal::_prev_mouse_y;
                }

                s32 mouse_button()
                {
                    return internal::_last_mouse_button_pressed;
                }

                f32 scroll_offset_x()
                {
                    return internal::_scroll_offset_x;
                }

                f32 scroll_offset_y()
                {
                    return internal::_scroll_offset_y;
                }

                bool is_left_button_pressed()
                {
                    bool r = is_mouse_button_pressed(internal::_window, GLFW_MOUSE_BUTTON_LEFT);

                    internal::_mouse_button_pressed[GLFW_MOUSE_BUTTON_LEFT] = r;
                    internal::_mouse_button_released[GLFW_MOUSE_BUTTON_LEFT] = false;

                    return r;
                }

                bool is_right_button_pressed()
                {
                    bool r = is_mouse_button_pressed(internal::_window, GLFW_MOUSE_BUTTON_RIGHT);

                    internal::_mouse_button_pressed[GLFW_MOUSE_BUTTON_RIGHT] = r;
                    internal::_mouse_button_released[GLFW_MOUSE_BUTTON_RIGHT] = false;

                    return r;
                }

                bool is_middle_button_pressed()
                {
                    bool r = is_mouse_button_pressed(internal::_window, GLFW_MOUSE_BUTTON_MIDDLE);

                    internal::_mouse_button_pressed[GLFW_MOUSE_BUTTON_MIDDLE] = r;
                    internal::_mouse_button_released[GLFW_MOUSE_BUTTON_MIDDLE] = false;

                    return r;
                }

                bool is_any_mouse_button_pressed()
                {
                    return std::any_of(internal::_mouse_button_pressed.begin(), internal::_mouse_button_pressed.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                bool is_any_mouse_button_released()
                {
                    return std::any_of(internal::_mouse_button_released.begin(), internal::_mouse_button_released.end(),
                        [](const auto& pair)
                    {
                        return pair.second;
                    });
                }

                void add_mouse_moved_callback(const std::function<void(f32, f32)>& callback)
                {
                    add_mouse_pos_callback(internal::_window, callback);
                }

                void add_mouse_dragged_callback(const std::function<void(f32, f32)>& callback)
                {
                    internal::_dragging_callback.push_back(callback);
                }

                void add_mouse_pressed_callback(const std::function<void(s32, s32)>& callback)
                {
                    add_mouse_button_pressed_callback(internal::_window, callback);
                }

                void add_mouse_released_callback(const std::function<void(s32, s32)>& callback)
                {
                    add_mouse_button_released_callback(internal::_window, callback);
                }

                void add_mouse_horizontal_wheel_callback(const std::function<void(f32)>& callback)
                {
                    add_mouse_scroll_callback(internal::_window,
                        [&callback](f32 xoffset, f32 yoffset)
                    {
                        callback(xoffset);
                    });
                }

                void add_mouse_vertical_wheel_callback(const std::function<void(f32)>& callback)
                {
                    add_mouse_scroll_callback(internal::_window,
                        [&callback](f32 xoffset, f32 yoffset)
                    {
                        callback(yoffset);
                    });
                }

                void request_pointer_lock()
                {
                    lock_cursor(internal::_window);
                }

                void request_pointer_hide()
                {
                    hide_cursor(internal::_window);
                }

                void request_pointer_unlock()
                {
                    unlock_cursor(internal::_window);
                }

                void request_pointer_show()
                {
                    show_cursor(internal::_window);
                }
            }
        }

        bool initialize(s32 w, s32 h)
        {
            // glfw: initialize and configure
            // ------------------------------
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_DEPTH_BITS, 24);

            #ifdef PPP_APPLE
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            #endif

            // glfw window creation
            // --------------------
            internal::_window = glfwCreateWindow(w, h, "Processing", NULL, NULL);
            if (internal::_window == NULL)
            {
                log::error("Failed to create GLFW window");

                terminate();
                return false;
            }

            glfwMakeContextCurrent(internal::_window);
            glfwSwapInterval(1);

            // Register the key pressed callback so we can cache the last key that was pressed
            input::keyboard::add_key_pressed_callback(
                [](s32 key, s32 scancode, s32 mods) 
            { 
                internal::_last_key_pressed = key; 
            });

            // Register the mouse button pressed callback so we can cache the last key that was pressed
            input::mouse::add_mouse_pressed_callback(
                [](s32 mouse_button, s32 mods)
            {
                internal::_last_mouse_button_pressed = mouse_button;
            });

            // Register the mouse move callback so we can check if we are dragging along the screen
            input::mouse::add_mouse_moved_callback(
                [](f32 xpos, f32 ypos)
            {
                if (input::mouse::is_any_mouse_button_pressed())
                {
                    for (const auto& c : internal::_dragging_callback)
                    {
                        c(xpos, ypos);
                    }
                }
            });

            // Register the mouse scroll callback so we can check if we are scrolling
            input::mouse::add_mouse_horizontal_wheel_callback(
                [](f32 xoffset)
            {
                internal::_scroll_offset_x = xoffset;
            });
            input::mouse::add_mouse_vertical_wheel_callback(
                [](f32 yoffset)
            {
                internal::_scroll_offset_y = yoffset;
            });

            internal::center_window(internal::_window);

            // Initialize frame time
            internal::_previous_frame_time = clock::now();

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
            internal::_prev_mouse_x = internal::_current_mouse_x;
            internal::_prev_mouse_y = internal::_current_mouse_y;
            internal::_current_mouse_x = input::mouse::mouse_x(internal::_window);
            internal::_current_mouse_y = input::mouse::mouse_y(internal::_window);

            internal::_scroll_offset_x = 0.0f;
            internal::_scroll_offset_y = 0.0f;

            auto current_frame_time = clock::now();

            internal::_delta_frame_time = clock::duration(internal::_previous_frame_time, current_frame_time);
            internal::_previous_frame_time = current_frame_time;

            if (internal::_is_looping)
            {
                internal::_frame_times[current_frame_index() % internal::_total_avg_frames] = delta_time();
            }
        }

        void window_width(s32* w)
        {
            s32 height = 0;
            glfwGetWindowSize(internal::_window, w, &height);
        }
        void window_height(s32* h)
        {
            s32 width = 0;
            glfwGetWindowSize(internal::_window, &width, h);
        }

        void loop()
        {
            internal::_is_looping = true;
        }

        void no_loop()
        {
            internal::_is_looping = false;
        }

        void redraw()
        {
            --internal::_current_frame_idx;
        }

        void present()
        {
            glfwSwapBuffers(internal::_window);

            // Make sure we don't overflow
            // Reset the frame indices to their initial value if we reach a certain threshold
            if (internal::_desired_frame_idx + 1 > std::numeric_limits<u32>::max())
            {
                internal::_current_frame_idx = 0;
                internal::_desired_frame_idx = 1;
            }

            internal::_current_frame_idx = internal::_desired_frame_idx;

            if (is_looping())
            {
                ++internal::_desired_frame_idx;
            }
        }

        void poll_events()
        {
            glfwPollEvents();
        }

        void request_quit()
        {
            glfwSetWindowShouldClose(internal::_window, true);
        }

        void target_frame_rate(u32 rate)
        {
            internal::_target_frame_rate = rate;
        }

        bool is_looping()
        {
            return internal::_is_looping;
        }

        bool can_draw()
        {
            return is_looping() || internal::_current_frame_idx < internal::_desired_frame_idx;
        }

        bool should_close()
        {
            return glfwWindowShouldClose(internal::_window);
        }

        u32 current_frame_index()
        {
            return internal::_current_frame_idx;
        }

        u32 desired_frame_index()
        {
            return internal::_desired_frame_idx;
        }

        u32 current_frame_rate()
        {
            if (internal::_is_looping)
            {
                return 1.0f / delta_time();
            }

            return 0.0f;
        }

        u32 average_frame_rate()
        {
            f64 total_frame_time = 0.0;
            for (s32 i = 0; i < internal::_total_avg_frames; ++i) 
            {
                total_frame_time += internal::_frame_times[i];
            }

            if (total_frame_time > 0.0) 
            {
                auto fps = internal::_total_avg_frames / total_frame_time;
                return fps;
            }

            return 0;
        }

        u32 target_frame_rate()
        {
            return internal::_target_frame_rate;
        }

        f32 max_frame_time()
        {
            return 1.0f / internal::_target_frame_rate;
        }

        f32 delta_time()
        {
            if (internal::_is_looping)
            {
                return std::chrono::duration<float>(internal::_delta_frame_time).count();
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