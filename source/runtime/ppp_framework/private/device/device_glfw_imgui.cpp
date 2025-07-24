#include "device/device_imgui.h"
#include "device/device.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

#include "imgui/implot/implot.h"

#include "GLFW/glfw3.h"

#include "util/log.h"

namespace ppp
{
    namespace imgui
    {      
        //-------------------------------------------------------------------------
        void init_imgui_context()
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImPlot::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

#if defined(PPP_OPENGL)
            auto glfw_window = reinterpret_cast<GLFWwindow*>(device::window());
            if (glfw_window)
            {
                // ImGUI will chain GLFW callbacks.
                // ImGUI callbacks will call the our previously installed callbacks, if any.
                // We hooked up a reroute ourselves so we don't have to install any of these callbacks.
                constexpr bool install_callbacks = false;

                ImGui_ImplGlfw_InitForOpenGL(glfw_window, install_callbacks);
            }
            else
            {
                log::error("Unable to convert device::window() to GLFW window");
                exit(-1);
            }
#endif
        }

        //-------------------------------------------------------------------------
        void shutdown_imgui_context()
        {
            ImGui_ImplGlfw_Shutdown();
            ImPlot::DestroyContext();
            ImGui::DestroyContext();
        }

        //-------------------------------------------------------------------------
        void step_imgui_context()
        {
            ImGui_ImplGlfw_NewFrame();
        }
    }
}