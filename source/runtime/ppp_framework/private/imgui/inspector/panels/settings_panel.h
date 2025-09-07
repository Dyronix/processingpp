#include "imgui/inspector/inspector_panel.h"
#include "imgui/inspector/inspector_panel_registry.h"
#include "imgui/inspector/inspector_profiler.h"
#include "imgui/imgui.h"
#include "imgui/imgui_icons_font_awesome.h"

#include "util/log.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //------------------------------------------------------------------------------
            // Inspector controls
            //------------------------------------------------------------------------------
            enum class camera_control_type : std::uint8_t
            {
                ORBIT = 0,	// CTRL + LMB/RMB + mouse move
                FREECAM		// CTRL + LMB/RMB + WASD
            };

            class settings_panel final : public ipanel
            {
            public:
                panel_info info() const override
                {
                    panel_info i = {};
                    i.id = "settings";
                    i.title = "Settings";
                    i.icon = ICON_FA_COG;
                    i.tooltip = "Settings";
                    i.toolbar_only = false;
                    i.toolbar_order = 20;
                    return i;
                }

                void on_draw(panel_context& ctx) override
                {
                    (void)ctx;

                    if (ImGui::CollapsingHeader("Logging Settings", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        bool logging_enabled = ppp::log::is_logging_enabled();
                        if (ImGui::Checkbox("Enable Logging", &logging_enabled))
                        {
                            if (logging_enabled)
                            {
                                on_enable_logging.emit();
                            }
                            else
                            {
                                on_disable_logging.emit();
                            }
                        }

                        static const char* level_names[] = { "Info", "Warning", "Error", "Critical" };
                        static int level = static_cast<int>(ppp::log::log_level::INFO); // set real level if stored

                        if (ImGui::Combo("Log Level", &level, level_names, IM_ARRAYSIZE(level_names)))
                        {
                            on_set_log_level.emit(static_cast<ppp::log::log_level>(level));
                        }
                    }

                    if (ImGui::CollapsingHeader("Camera Controls", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        using namespace ppp::imgui::inspector;

                        static int cam_mode = static_cast<int>(camera_control_type::ORBIT);

                        if (ImGui::RadioButton("Orbit", cam_mode == static_cast<int>(camera_control_type::ORBIT)))
                        {
                            cam_mode = static_cast<int>(camera_control_type::ORBIT);
                            on_set_camera_mode.emit(camera_control_type::ORBIT);
                        }

                        if (ImGui::RadioButton("Free Cam", cam_mode == static_cast<int>(camera_control_type::FREECAM)))
                        {
                            cam_mode = static_cast<int>(camera_control_type::FREECAM);
                            on_set_camera_mode.emit(camera_control_type::FREECAM);
                        }

                        if(cam_mode == static_cast<int>(camera_control_type::ORBIT))
                        {
                            float zoom = 1600.0f;
                            float panning = 5.0f;
                            float rotation = 50.0f;
                            float min_zoom = 1.0f;
                            float max_zoom = 600.0f;
                            bool invert = false;

                            if (ImGui::SliderFloat("Zoom Sensitivity", &zoom, 100.0f, 10000.0f, "%.1f"))
                            {
                                on_set_orbit_camera_zoom_speed.emit(zoom);
                            }
                            if (ImGui::SliderFloat("Panning Sensitivity", &panning, 1.0f, 100.0f, "%.1f"))
                            {
                                panning *= 0.01f;
                                on_set_orbit_camera_pan_speed.emit(panning);
                            }
                            if (ImGui::SliderFloat("Rotation Sensitivity", &rotation, 1.0f, 100.0f, "%.1f"))
                            {
                                rotation *= 0.01f;
                                on_set_orbit_camera_zoom_speed.emit(rotation);
                            }

                            if (ImGui::SliderFloat("Min Zoom", &min_zoom, 1.0f, 1000.0f, "%.1f"))
                            {
                                if (min_zoom > max_zoom)
                                    max_zoom = min_zoom; // keep valid
                                on_set_orbit_camera_min_zoom.emit(min_zoom);
                            }
                            if (ImGui::SliderFloat("Max Zoom", &max_zoom, 1.0f, 1000.0f, "%.1f"))
                            {
                                if (max_zoom < min_zoom)
                                    min_zoom = max_zoom; // keep valid
                                on_set_orbit_camera_max_zoom.emit(max_zoom);
                            }

                            if (ImGui::Checkbox("Invert Y Axis", &invert))
                            {
                                on_set_orbit_camera_invert_y.emit(invert);
                            }
                        }
                        else
                        {
                            float look = 1.0f;
                            float move = 5.0f;
                            float boost = 4.0f;

                            if (ImGui::SliderFloat("Look Sensitivity", &look, 1.0f, 100.0f, "%.1f"))
                            {
                                look *= 0.01f; // scale to more useful range
                                on_set_free_camera_look_speed.emit(look);
                            }
                            if (ImGui::SliderFloat("Move Sensitivity", &move, 1.0f, 100.0f, "%.1f"))
                            {
                                move *= 100.0f;
                                on_set_free_camera_move_speed.emit(move);
                            }
                            if (ImGui::SliderFloat("Boost Multiplier", &boost, 1.0f, 100.0f, "%.1f"))
                            {
                                on_set_free_camera_boost_multiplier.emit(boost);
                            }
                        }

                    }
                }

                inspector_signal<> on_enable_logging;
                inspector_signal<> on_disable_logging;
                inspector_signal<log::log_level> on_set_log_level;
                inspector_signal<camera_control_type> on_set_camera_mode;

                inspector_signal<f32> on_set_orbit_camera_pan_speed;
                inspector_signal<f32> on_set_orbit_camera_zoom_speed;
                inspector_signal<f32> on_set_orbit_camera_rotation_speed;
                inspector_signal<f32> on_set_orbit_camera_min_zoom;
                inspector_signal<f32> on_set_orbit_camera_max_zoom;
                inspector_signal<bool> on_set_orbit_camera_invert_y;

                inspector_signal<f32> on_set_free_camera_look_speed;
                inspector_signal<f32> on_set_free_camera_move_speed;
                inspector_signal<f32> on_set_free_camera_boost_multiplier;
            };
        }
    }
}