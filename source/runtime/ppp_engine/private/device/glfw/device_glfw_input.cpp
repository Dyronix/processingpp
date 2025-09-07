#include "device/device_input.h"
#include "device/device.h"

#include "render/render.h"

#include <unordered_set>

#include <GLFW/glfw3.h>

#define GLFW_MOUSE_SCROLL_X 0
#define GLFW_MOUSE_SCROLL_Y 1

namespace ppp
{
    namespace device
    {
        namespace input
        {
            struct input_context
            {
                std::unordered_map<u32, std::vector<key_callback>>                    key_callbacks;

                std::vector<mouse_pos_callback>                                       mouse_pos_callbacks;
                std::unordered_map<u32, std::vector<mouse_button_scroll_callback>>    mouse_scroll_callbacks;
                std::unordered_map<u32, std::vector<mouse_button_callback>>           mouse_button_callbacks;
            } g_ctx;

            namespace glfw
            {
                GLFWwindow* window()
                {
                    return reinterpret_cast<GLFWwindow*>(device::window());
                }

                void key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods)
                {
                    for (const auto& c : g_ctx.key_callbacks[action])
                    {
                        c(key, scancode, mods);
                    }
                }

                void mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods)
                {
                    for (const auto& c : g_ctx.mouse_button_callbacks[action])
                    {
                        c(button, mods);
                    }
                }

                void mouse_pos_callback(GLFWwindow* window, f64 xpos, f64 ypos)
                {
                    for (const auto& c : g_ctx.mouse_pos_callbacks)
                    {
                        c((f32)xpos, (f32)ypos);
                    }
                }

                void mouse_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset)
                {
                    if (xoffset != 0)
                    {
                        for (const auto& c : g_ctx.mouse_scroll_callbacks[GLFW_MOUSE_SCROLL_X])
                        {
                            c((f32)xoffset);
                        }
                    }

                    if (yoffset != 0)
                    {
                        for (const auto& c : g_ctx.mouse_scroll_callbacks[GLFW_MOUSE_SCROLL_Y])
                        {
                            c((f32)yoffset);
                        }
                    }
                }
            }

            void initialize()
            {
                glfwSetKeyCallback(glfw::window(), glfw::key_callback);

                glfwSetMouseButtonCallback(glfw::window(), glfw::mouse_button_callback);
                glfwSetCursorPosCallback(glfw::window(), glfw::mouse_pos_callback);
                glfwSetScrollCallback(glfw::window(), glfw::mouse_scroll_callback);
            }

            bool is_key_pressed(s32 code)
            {
                return glfwGetKey(glfw::window(), code) == GLFW_PRESS;
            }

            bool is_key_released(s32 code)
            {
                return glfwGetKey(glfw::window(), code) == GLFW_RELEASE;
            }

            bool is_key_down(s32 code)
            {
                int state = glfwGetKey(glfw::window(), code);

                return state == GLFW_PRESS || state == GLFW_REPEAT;
            }

            void add_key_pressed_callback(const key_pressed_callback& callback)
            {
                g_ctx.key_callbacks[GLFW_PRESS].push_back(callback);
            }

            void add_key_released_callback(const key_released_callback& callback)
            {
                g_ctx.key_callbacks[GLFW_RELEASE].push_back(callback);
            }

            void add_key_down_callback(const key_down_callback& callback)
            {
                g_ctx.key_callbacks[GLFW_REPEAT].push_back(callback);
            }

            f32 mouse_x()
            {
                f64 xpos, ypos;
                glfwGetCursorPos(glfw::window(), &xpos, &ypos);

                if (render::scissor_rect_enabled())
                {
                    f64 canvas_x = (f64)render::scissor_rect()->x;
                    f64 width = (f64)render::scissor_rect()->width;

                    xpos = xpos - canvas_x;
                }

                return (f32)xpos;
            }

            f32 mouse_y()
            {
                f64 xpos, ypos;
                glfwGetCursorPos(glfw::window(), &xpos, &ypos);

                if (render::scissor_rect_enabled())
                {
                    f64 canvas_y = (f64)render::scissor_rect()->y;
                    f64 height = (f64)render::scissor_rect()->height;

                    // Flip relative to scissor rect
                    ypos = canvas_y + height - ypos;
                }
                else
                {
                    s32 width, height;
                    glfwGetWindowSize(glfw::window(), &width, &height);

                    // Flip relative to full window
                    ypos = (f64)height - ypos;
                }

                return static_cast<f32>(ypos);
            }

            bool is_mouse_button_pressed(s32 code)
            {
                return glfwGetMouseButton(glfw::window(), code) == GLFW_PRESS;
            }

            bool is_mouse_button_released(s32 code)
            {
                return glfwGetMouseButton(glfw::window(), code) == GLFW_RELEASE;
            }

            bool is_mouse_button_down(s32 code)
            {
                int state = glfwGetMouseButton(glfw::window(), code);

                return state == GLFW_PRESS || state == GLFW_REPEAT;
            }

            void lock_cursor()
            {
                glfwSetInputMode(glfw::window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            void hide_cursor()
            {
                glfwSetInputMode(glfw::window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            }

            void unlock_cursor()
            {
                glfwSetInputMode(glfw::window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            void show_cursor()
            {
                glfwSetInputMode(glfw::window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            void add_mouse_pos_callback(const mouse_pos_callback& callback)
            {
                g_ctx.mouse_pos_callbacks.push_back(callback);
            }

            void add_mouse_button_pressed_callback(const mouse_button_pressed_callback& callback)
            {
                g_ctx.mouse_button_callbacks[GLFW_PRESS].push_back(callback);
            }

            void add_mouse_button_released_callback(const mouse_button_released_callback& callback)
            {
                g_ctx.mouse_button_callbacks[GLFW_RELEASE].push_back(callback);
            }

            void add_mouse_scroll_x_callback(const mouse_button_scroll_x_callback& callback)
            {
                g_ctx.mouse_scroll_callbacks[GLFW_MOUSE_SCROLL_X].push_back(callback);
            }

            void add_mouse_scroll_y_callback(const mouse_button_scroll_y_callback& callback)
            {
                g_ctx.mouse_scroll_callbacks[GLFW_MOUSE_SCROLL_Y].push_back(callback);
            }
        }
    }
}