#include "imgui/inspector/inspector_event_bus.h"

#include "imgui/inspector/panels/package_panel.h"
#include "imgui/inspector/panels/settings_panel.h"

#include "imgui/inspector/inspector_render_modes.h"
#include "imgui/inspector/inspector_view_modes.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            struct event_bus_context
            {
                // callbacks
                inspector_signal<bool> set_camera_invert_y_callback;
                inspector_signal<bool> set_show_static_callback;
                inspector_signal<bool> set_show_dynamic_callback;
                inspector_signal<bool> set_show_bounding_boxes_callback;
                inspector_signal<bool> set_show_grid_callback;
                inspector_signal<bool> pause_callback;
                inspector_signal<bool> unpause_callback;
                inspector_signal<bool> next_frame_callback;

                inspector_signal<float> set_orbit_camera_pan_speed_callback;
                inspector_signal<float> set_orbit_camera_zoom_speed_callback;
                inspector_signal<float> set_orbit_camera_rotation_speed_callback;
                inspector_signal<float> set_orbit_camera_min_zoom_callback;
                inspector_signal<float> set_orbit_camera_max_zoom_callback;
                inspector_signal<float> set_free_camera_look_speed_callback;
                inspector_signal<float> set_free_camera_move_speed_callback;
                inspector_signal<float> set_free_camera_boost_multiplier_callback;
                inspector_signal<float> set_fov_callback;

                inspector_signal<> inspector_draw_gui_callback;
                inspector_signal<> reload_images_callback;
                inspector_signal<> reload_shaders_callback;

                inspector_signal<render_mode> set_render_mode_callback;
                inspector_signal<view_direction> set_view_callback;
                inspector_signal<camera_control_type> set_camera_mode_callback;

                inspector_signal<package_info> cook_and_package_callback;
            };

            //-------------------------------------------------------------------------
            event_bus_context& event_bus()
            {
                static event_bus_context instance;
                return instance;
            }

            //-------------------------------------------------------------------------
            connection_id subscribe_pause(inspector_bool_delegate cb) { return event_bus().pause_callback.subscribe(cb); }
            connection_id subscribe_unpause(inspector_bool_delegate cb) { return event_bus().unpause_callback.subscribe(cb); }
            connection_id subscribe_next_frame(inspector_bool_delegate cb) { return event_bus().next_frame_callback.subscribe(cb); }
            connection_id subscribe_set_camera_invert_y(inspector_bool_delegate cb) { return event_bus().set_camera_invert_y_callback.subscribe(cb); }
            connection_id subscribe_set_show_static(inspector_bool_delegate cb) { return event_bus().set_show_static_callback.subscribe(cb); }
            connection_id subscribe_set_show_dynamic(inspector_bool_delegate cb) { return event_bus().set_show_dynamic_callback.subscribe(cb); }
            connection_id subscribe_set_show_bounding_boxes(inspector_bool_delegate cb) { return event_bus().set_show_bounding_boxes_callback.subscribe(cb); }
            connection_id subscribe_set_show_grid(inspector_bool_delegate cb) { return event_bus().set_show_grid_callback.subscribe(cb); }

            connection_id subscribe_set_orbit_camera_pan_speed(inspector_float_delegate cb) { return event_bus().set_orbit_camera_pan_speed_callback.subscribe(cb); }
            connection_id subscribe_set_orbit_camera_zoom_speed(inspector_float_delegate cb) { return event_bus().set_orbit_camera_zoom_speed_callback.subscribe(cb); }
            connection_id subscribe_set_orbit_camera_rotation_speed(inspector_float_delegate cb) { return event_bus().set_orbit_camera_rotation_speed_callback.subscribe(cb); }
            connection_id subscribe_set_orbit_camera_min_zoom(inspector_float_delegate cb) { return event_bus().set_orbit_camera_min_zoom_callback.subscribe(cb); }
            connection_id subscribe_set_orbit_camera_max_zoom(inspector_float_delegate cb) { return event_bus().set_orbit_camera_max_zoom_callback.subscribe(cb); }
            connection_id subscribe_set_free_camera_look_speed(inspector_float_delegate cb) { return event_bus().set_free_camera_look_speed_callback.subscribe(cb); }
            connection_id subscribe_set_free_camera_move_speed(inspector_float_delegate cb) { return event_bus().set_free_camera_move_speed_callback.subscribe(cb); }
            connection_id subscribe_set_free_camera_boost_multiplier(inspector_float_delegate cb) { return event_bus().set_free_camera_boost_multiplier_callback.subscribe(cb); }
            connection_id subscribe_set_fov(inspector_float_delegate cb) { return event_bus().set_fov_callback.subscribe(cb); }

            connection_id subscribe_inspector_draw_gui(inspector_void_delegate cb) { return event_bus().inspector_draw_gui_callback.subscribe(cb); }
            connection_id subscribe_reload_images(inspector_void_delegate cb) { return event_bus().reload_images_callback.subscribe(cb); }
            connection_id subscribe_reload_shaders(inspector_void_delegate cb) { return event_bus().reload_shaders_callback.subscribe(cb); }

            connection_id subscribe_set_render_mode(inspector_render_mode_delegate cb) { return event_bus().set_render_mode_callback.subscribe(cb); }
            connection_id subscribe_set_view_direction(inspector_view_delegate cb) { return event_bus().set_view_callback.subscribe(cb); }
            connection_id subscribe_set_camera_control_mode(inspector_camera_control_delegate cb) { return event_bus().set_camera_mode_callback.subscribe(cb); }
            connection_id subscribe_cook_and_package(inspector_cook_and_package_delegate cb) { return event_bus().cook_and_package_callback.subscribe(cb); }

            //-------------------------------------------------------------------------
            void emit_pause(bool v) { event_bus().pause_callback.emit(v); }
            void emit_unpause(bool v) { event_bus().unpause_callback.emit(v); }
            void emit_next_frame(bool v) { event_bus().next_frame_callback.emit(v); }
            void emit_set_camera_invert_y(bool v) { event_bus().set_camera_invert_y_callback.emit(v); }
            void emit_set_show_static(bool v) { event_bus().set_show_static_callback.emit(v); }
            void emit_set_show_dynamic(bool v) { event_bus().set_show_dynamic_callback.emit(v); }
            void emit_set_show_bounding_boxes(bool v) { event_bus().set_show_bounding_boxes_callback.emit(v); }
            void emit_set_show_grid(bool v) { event_bus().set_show_grid_callback.emit(v); }

            void emit_set_orbit_camera_pan_speed(float v) { event_bus().set_orbit_camera_pan_speed_callback.emit(v); }
            void emit_set_orbit_camera_zoom_speed(float v) { event_bus().set_orbit_camera_zoom_speed_callback.emit(v); }
            void emit_set_orbit_camera_rotation_speed(float v) { event_bus().set_orbit_camera_rotation_speed_callback.emit(v); }
            void emit_set_orbit_camera_min_zoom(float v) { event_bus().set_orbit_camera_min_zoom_callback.emit(v); }
            void emit_set_orbit_camera_max_zoom(float v) { event_bus().set_orbit_camera_max_zoom_callback.emit(v); }
            void emit_set_free_camera_move_speed(float v) { event_bus().set_free_camera_move_speed_callback.emit(v); }
            void emit_set_free_camera_look_speed(float v) { event_bus().set_free_camera_look_speed_callback.emit(v); }
            void emit_set_free_camera_boost_multiplier(float v) { event_bus().set_free_camera_boost_multiplier_callback.emit(v); }
            void emit_set_fov(float v) { event_bus().set_fov_callback.emit(v); }

            void emit_inspector_draw_gui() { event_bus().inspector_draw_gui_callback.emit(); }
            void emit_reload_images() { event_bus().reload_images_callback.emit(); }
            void emit_reload_shaders() { event_bus().reload_shaders_callback.emit(); }

            void emit_set_render_mode(render_mode v) { event_bus().set_render_mode_callback.emit(v); }
            void emit_set_view_direction(view_direction v) { event_bus().set_view_callback.emit(v); }
            void emit_set_camera_control_mode(camera_control_type v) { event_bus().set_camera_mode_callback.emit(v); }
            void emit_cook_and_package(const package_info& v) { event_bus().cook_and_package_callback.emit(v); }

            //-------------------------------------------------------------------------
            bool unsubscribe_inspector_signal(connection_id id)
            {
                bool result = false;

                if(!result) result = event_bus().pause_callback.unsubscribe(id);
                if(!result) result = event_bus().unpause_callback.unsubscribe(id);
                if(!result) result = event_bus().next_frame_callback.unsubscribe(id);
                if(!result) result = event_bus().inspector_draw_gui_callback.unsubscribe(id);
                if(!result) result = event_bus().reload_images_callback.unsubscribe(id);
                if(!result) result = event_bus().reload_shaders_callback.unsubscribe(id);
                if(!result) result = event_bus().set_render_mode_callback.unsubscribe(id);
                if(!result) result = event_bus().set_view_callback.unsubscribe(id);
                if(!result) result = event_bus().set_camera_mode_callback.unsubscribe(id);
                if(!result) result = event_bus().set_camera_invert_y_callback.unsubscribe(id);
                if(!result) result = event_bus().set_orbit_camera_pan_speed_callback.unsubscribe(id);
                if(!result) result = event_bus().set_orbit_camera_zoom_speed_callback.unsubscribe(id);
                if(!result) result = event_bus().set_orbit_camera_rotation_speed_callback.unsubscribe(id);
                if(!result) result = event_bus().set_orbit_camera_min_zoom_callback.unsubscribe(id);
                if(!result) result = event_bus().set_orbit_camera_max_zoom_callback.unsubscribe(id);
                if(!result) result = event_bus().set_free_camera_look_speed_callback.unsubscribe(id);
                if(!result) result = event_bus().set_free_camera_move_speed_callback.unsubscribe(id);
                if(!result) result = event_bus().set_free_camera_boost_multiplier_callback.unsubscribe(id);
                if(!result) result = event_bus().set_fov_callback.unsubscribe(id);
                if(!result) result = event_bus().set_show_static_callback.unsubscribe(id);
                if(!result) result = event_bus().set_show_dynamic_callback.unsubscribe(id);
                if(!result) result = event_bus().set_show_bounding_boxes_callback.unsubscribe(id);
                if(!result) result = event_bus().set_show_grid_callback.unsubscribe(id);
                if(!result) result = event_bus().cook_and_package_callback.unsubscribe(id);

                return result;
            }
        }
    }
}