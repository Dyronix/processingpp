#include "render/render_imgui.h"

#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_opengl3_loader.h"

namespace ppp
{
    namespace imgui
    {
        //-------------------------------------------------------------------------
        void init_imgui_renderer()
        {
            const char* glsl_version = "#version 130";

            ImGui_ImplOpenGL3_Init(glsl_version);
        }

        //-------------------------------------------------------------------------
        void shutdown_imgui_renderer()
        {
            ImGui_ImplOpenGL3_Shutdown();
        }

        //-------------------------------------------------------------------------
        void begin_draw_imgui_renderer()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();
        }

        //-------------------------------------------------------------------------
        void end_draw_imgui_renderer()
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    }
}