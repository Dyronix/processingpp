#include "device/device_input.h"
#include "render/render.h"

#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"

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
            namespace internal
            {
                static global_hash_map<u32, global_vector<keyboard::key_callback>>& key_callbacks()
                {
                    static auto s_key_callbacks = memory::tagged_placement_new<global_hash_map<u32, global_vector<keyboard::key_callback>>>();

                    return *s_key_callbacks;
                }

                static global_vector<mouse::mouse_pos_callback>& mouse_pos_callbacks()
                {
                    static auto s_mouse_pos_callbacks = memory::tagged_placement_new<global_vector<mouse::mouse_pos_callback>>();

                    return *s_mouse_pos_callbacks;
                }

                static global_hash_map<u32, global_vector<mouse::mouse_button_scroll_callback>>& mouse_scroll_callbacks()
                {
                    static auto s_mouse_scroll_callbacks = memory::tagged_placement_new<global_hash_map<u32, global_vector<mouse::mouse_button_scroll_callback>>>();

                    return *s_mouse_scroll_callbacks;
                }

                static global_hash_map<u32, global_vector<mouse::mouse_button_callback>>& mouse_button_callbacks()
                {
                    static auto s_mouse_button_callbacks = memory::tagged_placement_new<global_hash_map<u32, global_vector<mouse::mouse_button_callback>>>();

                    return *s_mouse_button_callbacks;
                }

                void key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods)
                {
                    for (const auto& c : key_callbacks()[action])
                    {
                        c(key, scancode, mods);
                    }
                }

                void mouse_pos_callback(GLFWwindow* window, f64 xpos, f64 ypos)
                {
                    for (const auto& c: mouse_pos_callbacks())
                    {
                        c((f32)xpos, (f32)ypos);
                    }
                }

                void mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods)
                {
                    for (const auto& c : mouse_button_callbacks()[action])
                    {
                        c(button, mods);
                    }
                }

                void mouse_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset)
                {
                    if (xoffset != 0)
                    {
                        for (const auto& c : mouse_scroll_callbacks()[GLFW_MOUSE_SCROLL_X])
                        {
                            c((f32)xoffset);
                        }
                    }

                    if (yoffset != 0)
                    {
                        for (const auto& c : mouse_scroll_callbacks()[GLFW_MOUSE_SCROLL_Y])
                        {
                            c((f32)yoffset);
                        }
                    }
                }
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
                    if (internal::key_callbacks().empty())
                    {
                        glfwSetKeyCallback(window, internal::key_callback);
                    }
                    internal::key_callbacks()[GLFW_PRESS].push_back(callback);
                }

                void add_key_released_callback(GLFWwindow* window, const key_released_callback& callback)
                {
                    if (internal::key_callbacks().empty())
                    {
                        glfwSetKeyCallback(window, internal::key_callback);
                    }
                    internal::key_callbacks()[GLFW_RELEASE].push_back(callback);
                }

                void add_key_down_callback(GLFWwindow* window, const key_down_callback& callback)
                {
                    if (internal::key_callbacks().empty())
                    {
                        glfwSetKeyCallback(window, internal::key_callback);
                    }
                    internal::key_callbacks()[GLFW_REPEAT].push_back(callback);
                }
            }

            namespace mouse
            {
                f32 mouse_x(GLFWwindow* window)
                {
                    f64 xpos, ypos;
                    glfwGetCursorPos(window, &xpos, &ypos);

                    if (render::scissor_enabled())
                    {
                        f64 canvas_x = (f64)render::scissor().x;
                        f64 width = (f64)render::scissor().w;

                        xpos = xpos - canvas_x;
                    }

                    return (f32)xpos;
                }

                f32 mouse_y(GLFWwindow* window)
                {
                    f64 xpos, ypos;
                    glfwGetCursorPos(window, &xpos, &ypos);

                    if (render::scissor_enabled())
                    {
                        f64 canvas_y = (f64)render::scissor().y;
                        f64 height = (f64)render::scissor().h;

                        ypos = ((ypos - canvas_y) - height) * -1.0;
                    }
                    else
                    {
                        s32 width, height;
                        glfwGetWindowSize(window, &width, &height);
                        ypos = ypos - (f64)height;
                    }

                    return (f32)ypos;
                }

                bool is_mouse_button_pressed(GLFWwindow* window, s32 code)
                {
                    return glfwGetMouseButton(window, code) == GLFW_PRESS;
                }
                
                bool is_mouse_button_released(GLFWwindow* window, s32 code)
                {
                    return glfwGetMouseButton(window, code) == GLFW_RELEASE;
                }

                bool is_mouse_button_down(GLFWwindow* window, s32 code)
                {
                    return glfwGetMouseButton(window, code) == GLFW_REPEAT;
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
                    internal::mouse_pos_callbacks().push_back(callback);
                    if (internal::mouse_pos_callbacks().empty())
                    {
                        glfwSetCursorPosCallback(window, internal::mouse_pos_callback);
                    }
                }

                void add_mouse_button_pressed_callback(GLFWwindow* window, const mouse_button_pressed_callback& callback)
                {
                    if (internal::mouse_button_callbacks().empty())
                    {
                        glfwSetMouseButtonCallback(window, internal::mouse_button_callback);
                    }
                    internal::mouse_button_callbacks()[GLFW_PRESS].push_back(callback);
                }

                void add_mouse_button_released_callback(GLFWwindow* window, const mouse_button_released_callback& callback)
                {
                    if (internal::mouse_button_callbacks().empty())
                    {
                        glfwSetMouseButtonCallback(window, internal::mouse_button_callback);
                    }
                    internal::mouse_button_callbacks()[GLFW_RELEASE].push_back(callback);
                }

                void add_mouse_scroll_x_callback(GLFWwindow* window, const mouse_button_scroll_x_callback& callback)
                {
                    if (internal::mouse_scroll_callbacks().empty())
                    {
                        glfwSetScrollCallback(window, internal::mouse_scroll_callback);
                    }
                    internal::mouse_scroll_callbacks()[GLFW_MOUSE_SCROLL_X].push_back(callback);
                }

                void add_mouse_scroll_y_callback(GLFWwindow* window, const mouse_button_scroll_y_callback& callback)
                {
                    if (internal::mouse_scroll_callbacks().empty())
                    {
                        glfwSetScrollCallback(window, internal::mouse_scroll_callback);
                    }
                    internal::mouse_scroll_callbacks()[GLFW_MOUSE_SCROLL_Y].push_back(callback);
                }
            }
        }
    }
}