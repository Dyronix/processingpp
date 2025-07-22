#include "device/device_imgui.h"
#include "device.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

#include "GLFW/glfw3.h"

namespace
{
    //-------------------------------------------------------------------------
    static void init_imgui_style(ppp::imgui::style_theme_type theme)
    {
        f32 main_scale_x, main_scale_y;
        ppp::device::primary_monitor_scale(&main_scale_x, &main_scale_y);

        // Setup Dear ImGui style
        switch(theme)
        {
            case ppp::imgui::style_theme_type::DARK: ImGui::StyleColorsDark();
            case ppp::imgui::style_theme_type::LIGHT: ImGui::StyleColorsLight();
            case ppp::imgui::style_theme_type::CLASSIC: ImGui::StyleColorsClassic();
        }

        // Setup scaling
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale_x);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale_x;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    #if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
        io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
    #endif

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }
}

namespace ppp
{
    namespace imgui
    {      
        //-------------------------------------------------------------------------
        void init_imgui_context(style_theme_type theme)
        {
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

            init_imgui_style(theme);

#if defined(PPP_OPENGL)
            ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWWindow*>(window), true);
#endif
        }
    }
}