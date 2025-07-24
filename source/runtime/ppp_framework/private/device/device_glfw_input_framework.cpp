#include "device/device_input_framework.h"
#include "device/device.h"

#include "util/log.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace device
    {
        GLFWkeyfun          g_prev_key = nullptr;
        GLFWmousebuttonfun  g_prev_mouse_btn = nullptr;
        GLFWcursorposfun    g_prev_mouse_pos = nullptr;
        GLFWscrollfun       g_prev_scroll = nullptr;

        bool                g_hooked = false;

        void key_callback(GLFWwindow* w, s32 key, s32 sc, s32 action, s32 mods)
        {
            if (ImGui::GetCurrentContext() != nullptr)
            {
                ImGui_ImplGlfw_KeyCallback(w, key, sc, action, mods);
                if (ImGui::GetIO().WantCaptureKeyboard) return;
            }
            if (g_prev_key) g_prev_key(w, key, sc, action, mods);
        }

        void mouse_button_callback(GLFWwindow* w, s32 btn, s32 action, s32 mods)
        {
            if (ImGui::GetCurrentContext() != nullptr)
            {
                ImGui_ImplGlfw_MouseButtonCallback(w, btn, action, mods);
                if (ImGui::GetIO().WantCaptureMouse) return;
            }
            if (g_prev_mouse_btn) g_prev_mouse_btn(w, btn, action, mods);
        }

        void mouse_pos_callback(GLFWwindow* w, f64 xpos, f64 ypos)
        {
            if (ImGui::GetCurrentContext() != nullptr)
            {
                ImGui_ImplGlfw_CursorPosCallback(w, xpos, ypos);
                if (ImGui::GetIO().WantCaptureMouse) return;
            }
            if (g_prev_mouse_pos) g_prev_mouse_pos(w, xpos, ypos);
        }

        void mouse_scroll_callback(GLFWwindow* w, f64 xoff, f64 yoff)
        {
            if (ImGui::GetCurrentContext() != nullptr)
            {
                ImGui_ImplGlfw_ScrollCallback(w, xoff, yoff);
                if (ImGui::GetIO().WantCaptureMouse) return;
            }
            if (g_prev_scroll) g_prev_scroll(w, xoff, yoff);
        }

        //-------------------------------------------------------------------------
        void reroute_input_callbacks()
        {
            if (g_hooked)
            {
                return;
            }

            g_hooked = true;

            auto glfw_window = reinterpret_cast<GLFWwindow*>(device::window());
            if (glfw_window)
            {
                g_prev_key = glfwSetKeyCallback(glfw_window, key_callback);

                g_prev_mouse_btn = glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
                g_prev_mouse_pos = glfwSetCursorPosCallback(glfw_window, mouse_pos_callback);
                g_prev_scroll = glfwSetScrollCallback(glfw_window, mouse_scroll_callback);
            }
            else
            {
                log::error("Unable to convert device::window() to GLFW window");
                exit(-1);
            }
        }
    }
}