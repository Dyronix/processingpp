#include "imgui/inspector/inspector_helpers.h"

#include "imgui/imgui_internal.h"

namespace
{
    //-------------------------------------------------------------------------
    static inline ImVec2 infer_size(ImVec2 content_size)
    {
        if (content_size.x <= 0.0f || content_size.y <= 0.0f)
        {
            // If there is an active window, use its size as a reasonable default.
            if (ImGui::GetCurrentWindow())
            {
                return ImGui::GetWindowSize();
            }
        }

        return content_size;
    }
}

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            void tooltip(const char* tooltip)
            {
                if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.6f)
                {
                    ImGui::BeginTooltip();
                    ImGui::SetTooltip("%s", tooltip);
                    ImGui::EndTooltip();
                }
            }

            //-------------------------------------------------------------------------
            ImVec2 anchor_position(anchor_type anchor, const ImVec2& content_size, const ImVec2& padding)
            {
                switch (anchor)
                {
                case anchor_type::TOP_LEFT:      return top_left(content_size, padding);
                case anchor_type::TOP_RIGHT:     return top_right(content_size, padding);
                case anchor_type::TOP_CENTER:    return top_center(content_size, padding);
                case anchor_type::BOTTOM_LEFT:   return bottom_left(content_size, padding);
                case anchor_type::BOTTOM_RIGHT:  return bottom_right(content_size, padding);
                case anchor_type::BOTTOM_CENTER: return bottom_center(content_size, padding);
                default:                         return top_left(content_size, padding);
                }
            }

            //-------------------------------------------------------------------------
            ImVec2 top_left(const ImVec2& /*content_size*/, const ImVec2& padding)
            {
                return ImVec2(padding.x, padding.y);
            }

            //-------------------------------------------------------------------------
            ImVec2 top_right(const ImVec2& content_size, const ImVec2& padding)
            {
                auto& io = ImGui::GetIO();
                ImVec2 new_content_size = infer_size(content_size);
                return ImVec2(io.DisplaySize.x - new_content_size.x - padding.x, padding.y);
            }

            //-------------------------------------------------------------------------
            ImVec2 top_center(const ImVec2& content_size, const ImVec2& padding)
            {
                auto& io = ImGui::GetIO();
                ImVec2 new_content_size = infer_size(content_size);
                return ImVec2((io.DisplaySize.x - new_content_size.x) * 0.5f, padding.y);
            }

            //-------------------------------------------------------------------------
            ImVec2 bottom_left(const ImVec2& /*content_size*/, const ImVec2& padding)
            {
                auto& io = ImGui::GetIO();
                return ImVec2(padding.x, io.DisplaySize.y - padding.y);
            }

            //-------------------------------------------------------------------------
            ImVec2 bottom_right(const ImVec2& content_size, const ImVec2& padding)
            {
                auto& io = ImGui::GetIO();
                ImVec2 new_content_size = infer_size(content_size);
                return ImVec2(io.DisplaySize.x - new_content_size.x - padding.x, io.DisplaySize.y - new_content_size.y - padding.y);
            }
            
            //-------------------------------------------------------------------------
            ImVec2 bottom_center(const ImVec2& content_size, const ImVec2& padding)
            {
                auto& io = ImGui::GetIO();
                ImVec2 new_content_size = infer_size(content_size);
                return ImVec2((io.DisplaySize.x - new_content_size.x) * 0.5f, io.DisplaySize.y - new_content_size.y - padding.y);
            }
        }
    }
}