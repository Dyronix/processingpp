#include "imgui/inspector/inspector.h"
#include "imgui/inspector/inspector_profiler.h"
#include "imgui/inspector/inspector_panel.h"
#include "imgui/inspector/inspector_panel_manager.h"
#include "imgui/inspector/inspector_panel_registry.h"
#include "imgui/inspector/inspector_notifications.h"
#include "imgui/inspector/inspector_theme.h"
#include "imgui/inspector/inspector_toolbar.h"
#include "imgui/inspector/inspector_overlays.h"
#include "imgui/inspector/inspector_visibility.h"
#include "imgui/inspector/inspector_runtime.h"
#include "imgui/inspector/inspector_render_modes.h"
#include "imgui/inspector/inspector_view_modes.h"
#include "imgui/inspector/inspector_event_bus.h"

#include "imgui/inspector/panels/about_panel.h"
#include "imgui/inspector/panels/package_panel.h"
#include "imgui/inspector/panels/profiler_panel.h"
#include "imgui/inspector/panels/reload_content_panel.h"
#include "imgui/inspector/panels/settings_panel.h"

#include "imgui/inspector/buttons/theme_button.h"
#include "imgui/inspector/buttons/close_button.h"

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

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            struct inspector_context
            {
                // panels
                bool right_panel_collapsed = false;

                // inspector
                bool show_inspector = false;

                // theme
                theme current_theme = theme::DARK;
                theme desired_theme = theme::DARK;

                // viewport commands
                inspector_overlays overlays;
                inspector_render_modes render_modes;
                inspector_view_modes view_modes;
                inspector_visibility visibility;
                inspector_runtime runtime;

                // panels
                panel_manager panel_mgr;
            };

            static inspector_context& ctx()
            {
                static inspector_context s_ctx;

                return s_ctx;
            }

            //-------------------------------------------------------------------------
            static void register_fonts()
            {
                f32 window_scale_x, window_scale_y;
                device::window_scale(&window_scale_x, &window_scale_y);
                const f32 ui_scale = window_scale_x;

                auto& io = ImGui::GetIO();
                io.Fonts->Clear();                              // ensure we rebuild for current DPI
                io.FontGlobalScale = 1.0f;

                ImFontConfig base_cfg;
                base_cfg.SizePixels = 13.0f * ui_scale;
                io.Fonts->AddFontDefault(&base_cfg);

                static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 }; // will not be copied by AddFont* so keep in scope.

                ImFontConfig config;
                config.MergeMode = true;
                config.PixelSnapH = true;
                config.OversampleH = 8;
                config.OversampleV = 8;
                config.GlyphMinAdvanceX = 16.0f;

                const f32 icon_size = 13.0f * ui_scale;
                auto font_awesome = vfs::resolve_path("local:content/fonts/FontAwesome5FreeSolid900.otf");
                if (!vfs::exists_filepath(font_awesome))
                {
                    log::critical("Could not find the font file FontAwesome5FreeSolid900.otf at path:{}", font_awesome);
                    return;
                }
                io.Fonts->AddFontFromFileTTF(font_awesome.c_str(), icon_size, &config, icons_ranges);
            }

            //-------------------------------------------------------------------------
            static void register_commands()
            {
                ctx().overlays.on_show_stats.subscribe([](bool show) { set_stats_visible(show); });
                ctx().overlays.on_show_drawcalls.subscribe([](bool show) { set_drawcalls_visible(show); });
                ctx().overlays.on_show_loaded_textures.subscribe([](bool show) { set_loaded_textures_visible(show); });
                ctx().overlays.on_show_cwd.subscribe([](bool show) { set_cwd_visible(show); });
                ctx().overlays.on_show_toolbar.subscribe([](bool show) { set_toolbar_visible(show); });
                ctx().overlays.on_change_camera_fov.subscribe([](f32 fov) { emit_set_fov(fov); });

                ctx().render_modes.on_render_mode_changed.subscribe([](render_mode mode) { emit_set_render_mode(mode); });
                ctx().view_modes.on_view_mode_changed.subscribe([](view_direction dir) { emit_set_view_direction(dir); });

                ctx().visibility.on_show_static_geometry.subscribe([](bool show) { emit_set_show_static(show); });
                ctx().visibility.on_show_dynamic_geometry.subscribe([](bool show) { emit_set_show_dynamic(show); });
                ctx().visibility.on_show_bounding_boxes.subscribe([](bool show) { emit_set_show_bounding_boxes(show); });
                ctx().visibility.on_show_grid.subscribe([](bool show) { emit_set_show_grid(show); });

                ctx().runtime.on_game_paused.subscribe([](bool paused) { emit_pause(paused); });
                ctx().runtime.on_game_unpaused.subscribe([](bool paused) { emit_unpause(paused); });
                ctx().runtime.on_next_frame.subscribe([]() { emit_next_frame(true); });
            }
            //-------------------------------------------------------------------------
            static void register_buttons()
            {
                // theme
                auto theme_bnt = panel_registry::get().register_panel<theme_button>();
                theme_bnt->on_theme_changed.subscribe([](theme t) { ctx().desired_theme = t; });
                // close
                auto close_btn = panel_registry::get().register_panel<close_button>();
                close_btn->on_close_inspector.subscribe([]() { ctx().show_inspector = false; });
            }
            //-------------------------------------------------------------------------
            static void register_panels()
            {
                // reload content
                auto reload_pnl = panel_registry::get().register_panel<reload_content_panel>();
                reload_pnl->on_reload_images.subscribe([]() { emit_reload_images(); });
                reload_pnl->on_reload_shaders.subscribe([]() { emit_reload_shaders(); });
                
                // profiler
                auto profiler_pnl = panel_registry::get().register_panel<profiler_panel>();
                
                // settings
                auto settings_pnl = panel_registry::get().register_panel<settings_panel>();
                settings_pnl->on_enable_logging.subscribe([]() {ppp::log::enable_logging(); });
                settings_pnl->on_disable_logging.subscribe([]() {ppp::log::disable_logging(); });
                settings_pnl->on_set_log_level.subscribe([](ppp::log::log_level level) { ppp::log::set_log_level(level); });
                settings_pnl->on_set_camera_mode.subscribe([](camera_control_type mode) { emit_set_camera_control_mode(mode); });
                settings_pnl->on_set_orbit_camera_pan_speed.subscribe([](f32 move) { emit_set_orbit_camera_pan_speed(move); });
                settings_pnl->on_set_orbit_camera_zoom_speed.subscribe([](f32 zoom) { emit_set_orbit_camera_zoom_speed(zoom); });
                settings_pnl->on_set_orbit_camera_rotation_speed.subscribe([](f32 zoom) { emit_set_orbit_camera_rotation_speed(zoom); });
                settings_pnl->on_set_orbit_camera_min_zoom.subscribe([](f32 zoom) { emit_set_orbit_camera_min_zoom(zoom); });
                settings_pnl->on_set_orbit_camera_max_zoom.subscribe([](f32 zoom) { emit_set_orbit_camera_max_zoom(zoom); });
                settings_pnl->on_set_orbit_camera_invert_y.subscribe([](bool invert) { emit_set_camera_invert_y(invert); });
                settings_pnl->on_set_free_camera_move_speed.subscribe([](f32 zoom) { emit_set_free_camera_move_speed(zoom); });
                settings_pnl->on_set_free_camera_look_speed.subscribe([](f32 zoom) { emit_set_free_camera_look_speed(zoom); });
                settings_pnl->on_set_free_camera_boost_multiplier.subscribe([](f32 zoom) { emit_set_free_camera_boost_multiplier(zoom); });
                
                // package
                auto package_pnl = panel_registry::get().register_panel<package_panel>();
                package_pnl->on_cook_and_package.subscribe([](const package_info& info) { emit_cook_and_package(info); });
                
                // about
                auto about_pnl = panel_registry::get().register_panel<about_panel>();
            }

            //-------------------------------------------------------------------------
            void set_pause_shortcut(key_code code)
            {
                add_key_pressed_callback(
                    [code](key_code key)
                    {
                        if (key == code)
                        {
                            if (ctx().runtime.is_paused())
                            {
                                emit_unpause(false);
                            }
                            else
                            {
                                emit_pause(true);
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
                            emit_next_frame(true);
                        }
                    });
            }

            //-------------------------------------------------------------------------
            void initialize()
            {
                register_fonts();

                apply_theme(ctx().current_theme);

                register_commands();
                register_buttons();
                register_panels();
            }
            //-------------------------------------------------------------------------
            void shutdown()
            {
                // Nothing to implement
            }
            //-------------------------------------------------------------------------
            void render(f32 dt)
            {
                f32 window_scale_x, window_scale_y;
                device::window_scale(&window_scale_x, &window_scale_y);

                static bool true_that = true;

                ImGuiWindowFlags flags =
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoScrollWithMouse |
                    ImGuiWindowFlags_AlwaysAutoResize;

                push_menu_theme(ctx().current_theme);

                if (ctx().show_inspector)
                {
                    // ----------------------------
                    // Top-left: Menu (hamburger + render/view/visibility)
                    // ----------------------------
                    {
                        ImVec2 overlay_size = ctx().overlays.draw(anchor_type::TOP_LEFT, { 0, 0 }, { 5 * window_scale_x , 5 * window_scale_y });
                        ImVec2 view_modes_size = ctx().view_modes.draw(anchor_type::TOP_LEFT, { 0, 0 }, ImVec2(10 * window_scale_x + overlay_size.x, 5 * window_scale_y));
                        ImVec2 render_modes_size = ctx().render_modes.draw(anchor_type::TOP_LEFT, { 0, 0 }, ImVec2(15 * window_scale_x + overlay_size.x + view_modes_size.x, 5 * window_scale_y));
                        ImVec2 visibility_size = ctx().visibility.draw(anchor_type::TOP_LEFT, { 0, 0 }, ImVec2(20 * window_scale_x + overlay_size.x + view_modes_size.x + render_modes_size.x, 5 * window_scale_y));
                    }

                    // ----------------------------
                    // Top-center: Play/Pause (and Next Frame)
                    // ----------------------------
                    {
                        ImVec2 runtime_size = ctx().runtime.draw(anchor_type::TOP_CENTER, { 0, 0 }, { 0, 5 * window_scale_y });
                    }

                    // ----------------------------
                    // Top-right: Collapsible inspector panel
                    // ----------------------------
                    {
                        if (!ctx().right_panel_collapsed)
                        {
                            // This has to be set outside of Begin/End
                            // We have an ImPlot widget that needs to have an explicit width
                            ImGui::SetNextWindowSize(ImVec2(400 * window_scale_x, 0));
                        }

                            ImGui::Begin("TR_Inspector", &true_that, flags);

                        if (!ctx().right_panel_collapsed)
                        {
                            ImGui::SetWindowPos(anchor_position(anchor_type::TOP_RIGHT, { 400 * window_scale_x, 0 }));
                        }
                        else
                        {
                            ImVec2 window_size = ImGui::GetWindowSize();
                            ImVec2 padding = { 5 * window_scale_x, 5 * window_scale_y };
                            ImGui::SetWindowPos(anchor_position(anchor_type::TOP_RIGHT, window_size, padding));
                        }

                        if (ctx().right_panel_collapsed)
                        {
                            // Unfold
                            {
                                if (ImGui::Button(ICON_FA_CHEVRON_LEFT))
                                {
                                    ctx().right_panel_collapsed = false;
                                }
                                tooltip("Open");
                            }
                        }
                        else
                        {
                            // Fold
                            {
                                if (ImGui::Button(ICON_FA_CHEVRON_RIGHT))
                                {
                                    ctx().right_panel_collapsed = true;
                                }
                                tooltip("Collapse");
                            }

                            ImGui::SameLine();

                            // User GUI
                            {
                                emit_inspector_draw_gui();
                            }

                            ImGui::SameLine();

                            {
                                panel_context pctx = {};
                                pctx.delta_time = dt;
                                ctx().panel_mgr.draw(pctx);
                            }
                        }

                        ImGui::End();
                    }

                    // ----------------------------
                    // Bottom-center : Stats / Toolbar
                    // ----------------------------
                    {
                        render_toolbar(dt);
                    }
                }
                else
                {
                    ImVec2 size_text = ImGui::CalcTextSize(ICON_FA_WRENCH);
                    size_text.x *= window_scale_x;
                    size_text.y *= window_scale_y;
                    ImGui::SetNextWindowPos(anchor_position(anchor_type::TOP_LEFT, size_text, { 5 * window_scale_x, 5 * window_scale_y }));

                    ImGui::Begin("TL_Inspector", &true_that, flags);

                    // Inspector wrech
                    {
                        if (ImGui::Button(ICON_FA_WRENCH))
                        {
                            ctx().show_inspector = true;
                        }
                        tooltip("Show Inspector");
                    }

                    ImGui::End();
                }

                render_notifications(dt);

                pop_menu_theme(ctx().current_theme);

                if (ctx().current_theme != ctx().desired_theme)
                {
                    ctx().current_theme = ctx().desired_theme;
                    apply_theme(ctx().current_theme);
                }
            }
        }
    }
}