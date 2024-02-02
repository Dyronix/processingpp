#include "device/device_input.h"

#include <unordered_set>

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace device
    {
        namespace input
        {
            namespace internal
            {
                std::unordered_map<u32, std::vector<std::function<void(s32, s32, s32)>>> _key_callbacks;

                void key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods)
                {
                    for (const auto& pair : _key_callbacks)
                    {
                        for (const auto& c : pair.second)
                        {
                            c(key, scancode, mods);
                        }
                    }
                }

                std::vector<std::function<void(s32, s32)>> _mouse_pos_callbacks;
                std::unordered_map<u32, std::vector<std::function<void(s32, s32)>>> _mouse_button_callbacks;
                std::vector<std::function<void(f32, f32)>> _mouse_scroll_callbacks;

                void mouse_pos_callback(GLFWwindow* window, f64 xpos, f64 ypos)
                {
                    for (const auto& c: _mouse_pos_callbacks)
                    {
                        c((s32)xpos, (s32)ypos);
                    }
                }

                void mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods)
                {
                    for (const auto& pair : _mouse_button_callbacks)
                    {
                        for (const auto& c : pair.second)
                        {
                            c(button, mods);
                        }
                    }
                }

                void mouse_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset)
                {
                    for (const auto& c : _mouse_scroll_callbacks)
                    {
                        c((f32)xoffset, (f32)yoffset);
                    }
                }

                s32 _scissor_x = -1;
                s32 _scissor_y = -1;
                s32 _scissor_width = -1;
                s32 _scissor_height = -1;
                bool _scissor_enable = false;
            }

            void push_canvas_dimensions(s32 x, s32 y, s32 width, s32 height)
            {
                internal::_scissor_x = x;
                internal::_scissor_y = y;
                internal::_scissor_width = width;
                internal::_scissor_height = height;
            }

            void push_canvas_enable(bool enable)
            {
                internal::_scissor_enable = enable;
            }

            namespace keyboard
            {
                bool is_key_pressed(GLFWwindow* window, s32 code)
                {
                    return glfwGetKey(window, code) == GLFW_PRESS;
                }

                bool is_key_released(GLFWwindow* window, s32 code)
                {
                    return glfwGetKey(window, code) == GLFW_RELEASE;
                }

                bool is_key_down(GLFWwindow* window, s32 code)
                {
                    return glfwGetKey(window, code) == GLFW_REPEAT;
                }

                void add_key_pressed_callback(GLFWwindow* window, const key_pressed_callback& callback)
                {
                    if (internal::_key_callbacks.empty())
                    {
                        glfwSetKeyCallback(window, internal::key_callback);
                    }
                    internal::_key_callbacks[GLFW_PRESS].push_back(callback);
                }

                void add_key_released_callback(GLFWwindow* window, const key_released_callback& callback)
                {
                    if (internal::_key_callbacks.empty())
                    {
                        glfwSetKeyCallback(window, internal::key_callback);
                    }
                    internal::_key_callbacks[GLFW_RELEASE].push_back(callback);
                }

                void add_key_down_callback(GLFWwindow* window, const key_down_callback& callback)
                {
                    if (internal::_key_callbacks.empty())
                    {
                        glfwSetKeyCallback(window, internal::key_callback);
                    }
                    internal::_key_callbacks[GLFW_REPEAT].push_back(callback);
                }
            }

            namespace mouse
            {
                s32 mouse_x(GLFWwindow* window)
                {
                    f64 xpos, ypos;
                    glfwGetCursorPos(window, &xpos, &ypos);

                    if (internal::_scissor_enable)
                    {
                        s32 canvas_x = internal::_scissor_x;
                        s32 width = internal::_scissor_width;

                        xpos = xpos - canvas_x;
                    }

                    return (s32)xpos;
                }

                s32 mouse_y(GLFWwindow* window)
                {
                    f64 xpos, ypos;
                    glfwGetCursorPos(window, &xpos, &ypos);

                    if (internal::_scissor_enable)
                    {
                        s32 canvas_y = internal::_scissor_y;
                        s32 height = internal::_scissor_height;

                        ypos = ((ypos - canvas_y) - height) * -1;
                    }
                    else
                    {
                        s32 width, height;
                        glfwGetWindowSize(window, &width, &height);
                        ypos = ypos - height;
                    }

                    return (s32)ypos;
                }

                bool is_mouse_button_pressed(GLFWwindow* window, s32 code)
                {
                    return glfwGetMouseButton(window, code) == GLFW_PRESS;
                }
                
                bool is_mouse_button_released(GLFWwindow* window, s32 code)
                {
                    return glfwGetMouseButton(window, code) == GLFW_RELEASE;
                }

                void lock_cursor(GLFWwindow* window)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }

                void hide_cursor(GLFWwindow* window)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                }

                void unlock_cursor(GLFWwindow* window)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }

                void show_cursor(GLFWwindow* window)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }

                void add_mouse_pos_callback(GLFWwindow* window, const mouse_pos_callback& callback)
                {
                    internal::_mouse_pos_callbacks.push_back(callback);
                    if (internal::_mouse_pos_callbacks.empty())
                    {
                        glfwSetCursorPosCallback(window, internal::mouse_pos_callback);
                    }
                }

                void add_mouse_button_pressed_callback(GLFWwindow* window, const mouse_button_pressed_callback& callback)
                {
                    if (internal::_mouse_button_callbacks.empty())
                    {
                        glfwSetMouseButtonCallback(window, internal::mouse_button_callback);
                    }
                    internal::_mouse_button_callbacks[GLFW_PRESS].push_back(callback);
                }

                void add_mouse_button_released_callback(GLFWwindow* window, const mouse_button_released_callback& callback)
                {
                    if (internal::_mouse_button_callbacks.empty())
                    {
                        glfwSetMouseButtonCallback(window, internal::mouse_button_callback);
                    }
                    internal::_mouse_button_callbacks[GLFW_RELEASE].push_back(callback);
                }

                void add_mouse_scroll_callback(GLFWwindow* window, const mouse_button_scroll_callback& callback)
                {
                    if (internal::_mouse_scroll_callbacks.empty())
                    {
                        glfwSetScrollCallback(window, internal::mouse_scroll_callback);
                    }
                    internal::_mouse_scroll_callbacks.push_back(callback);
                }
            }
        }
    }
}