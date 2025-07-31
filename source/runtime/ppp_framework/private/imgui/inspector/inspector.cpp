#include "imgui/inspector/inspector.h"
#include "imgui/inspector/inspector_profiler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_icons_font_awesome.h"

#include "imgui/implot/implot.h"

#include "fileio/fileio.h"
#include "fileio/vfs.h"

#include "device/device.h"

#include "render/render.h"

#include "events.h"
#include "environment.h"
#include "structure.h"

namespace
{
	//-------------------------------------------------------------------------
	static void tooltip(const char* tooltip)
	{
		if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.6f)
		{
			ImGui::BeginTooltip();
			ImGui::SetTooltip("%s", tooltip);
			ImGui::EndTooltip();
		}
	}
}

namespace ppp
{
	namespace imgui
	{
		namespace inspector
		{
			//-------------------------------------------------------------------------
			static void push_menu_theme();
			//-------------------------------------------------------------------------
			static void pop_menu_theme();
			//-------------------------------------------------------------------------
			static void embrace_the_darkness();
			//-------------------------------------------------------------------------
			static void follow_the_light();
			//-------------------------------------------------------------------------
			static void go_gray();

			//-------------------------------------------------------------------------
			enum class theme : std::uint8_t
			{
				DARK = 0,
				LIGHT = 1,
				GRAY = 2,
			};

			constexpr static s32 g_total_themes = 3;

			//-------------------------------------------------------------------------
			struct notification
			{
				s32	id;
				notification_type type;
				std::string message;
				f32	time;
			};

			//-------------------------------------------------------------------------
			struct inspector_context
			{
				std::vector<notification> notifications;

				bool game_paused = false;

				bool show_profiler = false;
				bool show_about = false;
				bool show_settings = false;
				bool show_inspector = false;

				theme current_theme = theme::DARK;

				inspector_delegate pause_callback;
				inspector_delegate unpause_callback;
				inspector_delegate next_frame_callback;
				inspector_inspector_gui_delegate inspector_gui_callback;
				inspector_reload_images_delegate reload_images_callback;
			};

			static inspector_context& ctx()
			{
				static inspector_context s_ctx;

				return s_ctx;
			}

			//-------------------------------------------------------------------------
			static void apply_theme()
			{
				switch (ctx().current_theme)
				{
				case theme::DARK:
					embrace_the_darkness();
					break;
				case theme::LIGHT:
					follow_the_light();
					break;
				case theme::GRAY:
					go_gray();
					break;
				}
			}
			//-------------------------------------------------------------------------
			void push_menu_theme()
			{
				switch (ctx().current_theme)
				{
				case theme::DARK:
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.00f));
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
					break;
				case theme::LIGHT:
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.00f));
					break;
				case theme::GRAY:
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.00f));
					break;
				}
			}
			//-------------------------------------------------------------------------
			void pop_menu_theme()
			{
				switch (ctx().current_theme)
				{
				case theme::DARK:
					ImGui::PopStyleVar(1);
					ImGui::PopStyleColor(1);
					break;
				case theme::LIGHT:
					ImGui::PopStyleColor(1);
					break;
				case theme::GRAY:
					ImGui::PopStyleColor(1);
					break;
				}
			}

			//-------------------------------------------------------------------------
			void embrace_the_darkness()
			{
				ImVec4* colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
				colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
				colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
				colors[ImGuiCol_Button] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.80f, 0.80f, 0.54f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
				colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
				colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
				colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
				colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
				colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
				colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);

				ImGuiStyle& style = ImGui::GetStyle();
				style.WindowPadding = ImVec2(8.00f, 8.00f);
				style.FramePadding = ImVec2(5.00f, 2.00f);
				style.ItemSpacing = ImVec2(6.00f, 6.00f);
				style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
				style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
				style.IndentSpacing = 25;
				style.ScrollbarSize = 15;
				style.GrabMinSize = 10;
				style.WindowBorderSize = 1;
				style.ChildBorderSize = 1;
				style.PopupBorderSize = 1;
				style.FrameBorderSize = 1;
				style.TabBorderSize = 1;
				style.WindowRounding = 7;
				style.ChildRounding = 4;
				style.FrameRounding = 3;
				style.PopupRounding = 4;
				style.ScrollbarRounding = 9;
				style.GrabRounding = 3;
				style.LogSliderDeadzone = 4;
				style.TabRounding = 4;
				style.FrameBorderSize = 0;
			}
			//-------------------------------------------------------------------------
			void follow_the_light()
			{
				ImVec4* colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
				colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
				colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
				colors[ImGuiCol_Button] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
				colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
				colors[ImGuiCol_Tab] = ImVec4(0.76f, 0.80f, 0.84f, 0.93f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
				colors[ImGuiCol_TabActive] = ImVec4(0.60f, 0.73f, 0.88f, 1.00f);
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

				ImGuiStyle& style = ImGui::GetStyle();
				style.WindowBorderSize = 0;
				style.ChildBorderSize = 0;
				style.PopupBorderSize = 0;
				style.FrameBorderSize = 0;
				style.TabBorderSize = 0;

				style.IndentSpacing = 25;
				style.ScrollbarSize = 10;
				style.GrabMinSize = 10;
				style.WindowBorderSize = 1;
				style.ChildBorderSize = 0;
				style.PopupBorderSize = 0;
				style.FrameBorderSize = 0;
				style.TabBorderSize = 1;
				style.WindowRounding = 7;
				style.ChildRounding = 4;
				style.FrameRounding = 3;
				style.PopupRounding = 4;
				style.ScrollbarRounding = 9;
				style.GrabRounding = 3;
				style.LogSliderDeadzone = 4;
				style.TabRounding = 4;
				style.FrameBorderSize = 0;
			}
			//-------------------------------------------------------------------------
			void go_gray()
			{
				ImVec4* colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
				colors[ImGuiCol_Border] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.24f, 0.24f, 0.24f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.26f, 0.54f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.54f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.63f, 0.63f, 0.63f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
				colors[ImGuiCol_Button] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.63f, 0.63f, 0.63f, 1.00f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
				colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
				colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
				colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
				colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
				colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
				colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.50f, 0.50f, 0.50f, 0.5f);

				ImGuiStyle& style = ImGui::GetStyle();
				style.WindowPadding = ImVec2(8.00f, 8.00f);
				style.FramePadding = ImVec2(5.00f, 2.00f);
				style.ItemSpacing = ImVec2(6.00f, 6.00f);
				style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
				style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
				style.IndentSpacing = 25;
				style.ScrollbarSize = 10;
				style.GrabMinSize = 10;
				style.WindowBorderSize = 1;
				style.ChildBorderSize = 0;
				style.PopupBorderSize = 0;
				style.FrameBorderSize = 0;
				style.TabBorderSize = 1;
				style.WindowRounding = 7;
				style.ChildRounding = 4;
				style.FrameRounding = 3;
				style.PopupRounding = 4;
				style.ScrollbarRounding = 9;
				style.GrabRounding = 3;
				style.LogSliderDeadzone = 4;
				style.TabRounding = 4;
				style.FrameBorderSize = 0;
			}

			//-------------------------------------------------------------------------
			void subscribe_pause(inspector_delegate callback)
			{
				ctx().pause_callback = callback;
			}
			//-------------------------------------------------------------------------
			void subscribe_unpause(inspector_delegate callback)
			{
				ctx().unpause_callback = callback;
			}
			//-------------------------------------------------------------------------
			void subscribe_next_frame(inspector_delegate callback)
			{
				ctx().next_frame_callback = callback;
			}
			//-------------------------------------------------------------------------
			void subscribe_inspector_gui(inspector_inspector_gui_delegate callback)
			{
				ctx().inspector_gui_callback = callback;
			}
			//-------------------------------------------------------------------------
			void subscribe_reload_images(inspector_reload_images_delegate callback)
			{
				ctx().reload_images_callback = callback;
			}

			//-------------------------------------------------------------------------
			void set_pause_shortcut(key_code code)
			{
				add_key_pressed_callback(
					[code](key_code key)
				{
					if (key == code)
					{
						ctx().game_paused = !ctx().game_paused;

						// propagate to the device
						if (ctx().game_paused)
						{
							if (ctx().pause_callback)
							{
								ctx().pause_callback(ctx().game_paused);
							}
						}
						else
						{
							if (ctx().unpause_callback)
							{
								ctx().unpause_callback(ctx().game_paused);
							}
						}
					}
				});
			}
			//-------------------------------------------------------------------------
			void set_nextframe_shortcut(key_code code)
			{
				add_key_pressed_callback(
					[code](key_code key)
				{
					if (key == code)
					{
						if (ctx().next_frame_callback)
						{
							ctx().next_frame_callback(true);
						}
					}
				});
			}

			//-------------------------------------------------------------------------
			void initialize()
			{
				f32 window_scale_x, window_scale_y;
				device::window_scale(&window_scale_x, &window_scale_y);

				auto& io = ImGui::GetIO();
				io.Fonts->AddFontDefault();

				const f32 ui_scale = window_scale_x;
				const f32 icon_size = 12.0f;

				static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 }; // will not be copied by AddFont* so keep in scope.

				ImFontConfig config;
				config.MergeMode = true;
				config.PixelSnapH = true;
				config.PixelSnapV = true;
				config.OversampleH = 8;
				config.OversampleV = 8;

				auto font_awesome = vfs::resolve_path("local:content/fonts/FontAwesome5FreeSolid900.otf");
				if (!vfs::exists_filepath(font_awesome))
				{
					log::critical("Could not find the font file FontAwesome5FreeSolid900.otf at path:{}", font_awesome);
					return;
				}
				io.Fonts->AddFontFromFileTTF(font_awesome.c_str(), icon_size * ui_scale, &config, icons_ranges);

				apply_theme();
			}
			//-------------------------------------------------------------------------
			void shutdown()
			{
				// Nothing to implement
			}
			//-------------------------------------------------------------------------
			void render(f32 dt)
			{
				bool true_that = true;
				auto& io = ImGui::GetIO();

				theme new_theme = ctx().current_theme;
				push_menu_theme();

				if (ctx().show_profiler ||
					ctx().show_about ||
					ctx().show_inspector)
				{
					ImGui::Begin("Inspector", nullptr,
						ImGuiWindowFlags_NoScrollbar |
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoResize |
						ImGuiWindowFlags_NoMove |
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoSavedSettings |
						ImGuiWindowFlags_NoScrollWithMouse);
					ImGui::SetWindowPos({ 5, 5 });
					ImGui::SetWindowSize({ -1, -1 });

					// Runtime
					{
						if (ctx().game_paused)
						{
							if (ImGui::Button(ICON_FA_PLAY))
							{
								ctx().game_paused = false;

								if (ctx().unpause_callback)
								{
									ctx().unpause_callback(ctx().game_paused);
								}
							}
							tooltip("Play");

							ImGui::SameLine();

							if (ImGui::Button(ICON_FA_FAST_FORWARD))
							{
								if (ctx().next_frame_callback)
								{
									ctx().next_frame_callback(true);
								}
							}
							tooltip("Next Frame");
						}
						else
						{
							if (ImGui::Button(ICON_FA_PAUSE))
							{
								ctx().game_paused = true;

								if (ctx().pause_callback)
								{
									ctx().pause_callback(ctx().game_paused);
								}
							}
							tooltip("Pause");
						}
					}

					ImGui::SameLine();

					// Profiler
					{
						if (ImGui::Button(ICON_FA_CHART_BAR))
						{
							ctx().show_profiler = !ctx().show_profiler;
						}
						tooltip("Profiler");

						ImGui::SameLine();

						if (ImGui::Button(ICON_FA_IMAGES))
						{
							if (ctx().reload_images_callback)
							{
								ctx().reload_images_callback();
							}
						}
						tooltip("Reload Art");
					}

					ImGui::SameLine();

					// Theme
					{
						if (ImGui::Button(ICON_FA_ADJUST))
						{
							s32 t = (s32)ctx().current_theme;
							t = (t + 1) % g_total_themes;
							new_theme = (theme)t;
						}
						tooltip("Theme");
					}

					ImGui::SameLine();

					// Settings
					{
						if (ImGui::Button(ICON_FA_COG))
						{
							ctx().show_settings = true;
						}
						tooltip("Settings");
					}

					ImGui::SameLine();

					// User code
					{
						if (ctx().inspector_gui_callback)
						{
							ctx().inspector_gui_callback();
						}
					}

					ImGui::SameLine();

					// About
					{
						if (ImGui::Button(ICON_FA_QUESTION_CIRCLE))
						{
							ctx().show_about = true;
						}
						tooltip("About");
					}

					ImGui::SameLine();

					// Quit
					{
						if (ImGui::Button(ICON_FA_POWER_OFF))
						{
							quit();
						}
						tooltip("Quit Application");
					}

					ImGui::SameLine();

					// Close
					{
						if (ImGui::Button(ICON_FA_TIMES))
						{
							ctx().show_inspector = false;
						}
						tooltip("Close Inspector");
					}

					ImGui::End();

					{	// Bottom stats
						const auto& io = ImGui::GetIO();
						ImGui::Begin("Stats", &true_that,
							ImGuiWindowFlags_NoScrollbar |
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoCollapse |
							ImGuiWindowFlags_NoSavedSettings |
							ImGuiWindowFlags_NoScrollWithMouse);
						ImGui::SetWindowPos({ 5, io.DisplaySize.y - ImGui::GetWindowHeight() - 5.0f });
						ImGui::SetWindowSize({ -1, -1 });

						auto stats = render::stats();
						auto batched_draw_calls = std::to_string(stats.batched_draw_calls);
						auto instanced_draw_calls = std::to_string(stats.instanced_draw_calls);
						auto textures = std::to_string(stats.textures);

						ImGui::Text("%.3f ms | FPS: %.1f | batched draw calls:%s | instanced draw calls:%s | textures:%s | cwd:%s",
							1000.0f / io.Framerate,
							io.Framerate,
							batched_draw_calls.c_str(),
							instanced_draw_calls.c_str(),
							textures.c_str(),
							cwd().data());

						ImGui::End();
					}

				}
				else
				{
					ImGui::Begin("Inspector", nullptr,
						ImGuiWindowFlags_NoScrollbar |
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoResize |
						ImGuiWindowFlags_NoMove |
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoSavedSettings |
						ImGuiWindowFlags_NoScrollWithMouse);
					ImGui::SetWindowPos({ 5, 5 });
					ImGui::SetWindowSize({ -1, -1 });

					// A small button to show the inspector
					if (ImGui::Button(ICON_FA_WRENCH))
					{
						ctx().show_inspector = true;
					}
					tooltip("Show Inspector");

					ImGui::End();
				}

				{	// Notifications
					f32 x = -10.0f;
					f32 y = 10.0f;
					for (auto& n : ctx().notifications)
					{
						ImGui::Begin(std::to_string(n.id).c_str(), &true_that,
							ImGuiWindowFlags_NoScrollbar |
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoCollapse |
							ImGuiWindowFlags_NoSavedSettings |
							ImGuiWindowFlags_NoScrollWithMouse);
						ImGui::SetWindowPos({ io.DisplaySize.x - ImGui::GetWindowWidth() + x, y });
						
						// Draw the notification icon
						ImVec4 color;
						std::string icon;
						switch (n.type)
						{
						case notification_type::INFO:
							color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
							icon = ICON_FA_INFO_CIRCLE;
							break;
						case notification_type::SUCCESS:
							color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
							icon = ICON_FA_CHECK_CIRCLE;
							break;
						case notification_type::WARNING:
							color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
							icon = ICON_FA_EXCLAMATION_TRIANGLE;
							break;
						case notification_type::ERROR:
							color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
							icon = ICON_FA_TIMES_CIRCLE;
							break;
						}

						ImGui::PushStyleColor(ImGuiCol_Text, color);
						ImGui::Button(icon.c_str());
						ImGui::PopStyleColor(1);
						ImGui::SameLine();
						ImGui::Text("%s", n.message.c_str());
						y += ImGui::GetWindowHeight() + 5.0f;
						ImGui::End();
						n.time -= (f32)dt;
					}

					ctx().notifications.erase(std::remove_if(ctx().notifications.begin(), ctx().notifications.end(),
						[](const notification& n)
					{
						return n.time <= 0.0f;
					}), ctx().notifications.end());
				}

				pop_menu_theme();

				if (ctx().show_profiler)
				{
					inspect_profiler(ctx().show_profiler);
				}

				if (ctx().show_about)
				{
					ImGui::Begin("About", &ctx().show_about);
					ImGui::Text(" An attempt to create P5 processing in C++ ");
					ImGui::End();
				}

				if (ctx().show_settings)
				{
					ImGui::Begin("Settings", &ctx().show_settings);

					if (ImGui::CollapsingHeader("Logging Settings", ImGuiTreeNodeFlags_DefaultOpen))
					{
						// Enable/disable logging
						bool logging_enabled = ppp::log::is_logging_enabled();
						if (ImGui::Checkbox("Enable Logging", &logging_enabled))
						{
							if (logging_enabled)
							{
								ppp::log::enable_logging();
							}
							else
							{
								ppp::log::disable_logging();
							}
						}

						// Select log level
						static const char* log_level_names[] = { "Info", "Warning", "Error", "Critical" };
						static int current_log_level = static_cast<int>(ppp::log::log_level::INFO); // You might want to fetch the real current level if stored

						if (ImGui::Combo("Log Level", &current_log_level, log_level_names, IM_ARRAYSIZE(log_level_names)))
						{
							ppp::log::set_log_level(static_cast<ppp::log::log_level>(current_log_level));
						}
					}

					ImGui::End();
				}

				// ImGUI::Render ...

				if (new_theme != ctx().current_theme)
				{
					ctx().current_theme = new_theme;
					apply_theme();
				}
			}

			//-------------------------------------------------------------------------
			s32 notify(notification_type type, const std::string& message, f32 time)
			{
				static s32 id = 0;
				ctx().notifications.push_back({ id, type, message, time });
				return id++;
			}
		}
	}
}