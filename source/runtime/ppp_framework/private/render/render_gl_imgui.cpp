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
    }
}