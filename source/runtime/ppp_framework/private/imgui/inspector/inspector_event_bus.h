#pragma once

#include "imgui/inspector/inspector_signal.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
			//-------------------------------------------------------------------------
			enum class render_mode : std::uint8_t;
			enum class view_direction : std::uint8_t;
			enum class camera_control_type : std::uint8_t;

			struct package_info;

			//-------------------------------------------------------------------------
			/**
			 * @brief C-style function pointer type for inspector event callbacks.
			 */
			using inspector_bool_delegate = void(*)(bool value);
			using inspector_float_delegate = void(*)(float value);
			using inspector_void_delegate = void(*)();
			using inspector_render_mode_delegate = void(*)(render_mode mode);
			using inspector_view_delegate = void(*)(view_direction dir);
			using inspector_camera_control_delegate = void(*)(camera_control_type type);
			using inspector_cook_and_package_delegate = void(*)(const package_info& info);

			//-------------------------------------------------------------------------
            /**
			 * @brief Subscription functions for engine events
			 */
			connection_id subscribe_pause(inspector_bool_delegate cb);
			connection_id subscribe_unpause(inspector_bool_delegate cb);
			connection_id subscribe_next_frame(inspector_bool_delegate cb);
			connection_id subscribe_set_camera_invert_y(inspector_bool_delegate cb);
			connection_id subscribe_set_show_static(inspector_bool_delegate cb);
			connection_id subscribe_set_show_dynamic(inspector_bool_delegate cb);
			connection_id subscribe_set_show_bounding_boxes(inspector_bool_delegate cb);
			connection_id subscribe_set_show_grid(inspector_bool_delegate cb);

			connection_id subscribe_set_orbit_camera_pan_speed(inspector_float_delegate cb);
			connection_id subscribe_set_orbit_camera_zoom_speed(inspector_float_delegate cb);
			connection_id subscribe_set_orbit_camera_rotation_speed(inspector_float_delegate cb);
			connection_id subscribe_set_orbit_camera_min_zoom(inspector_float_delegate cb);
			connection_id subscribe_set_orbit_camera_max_zoom(inspector_float_delegate cb);
			connection_id subscribe_set_free_camera_look_speed(inspector_float_delegate cb);
			connection_id subscribe_set_free_camera_move_speed(inspector_float_delegate cb);
			connection_id subscribe_set_free_camera_boost_multiplier(inspector_float_delegate cb);
			connection_id subscribe_set_fov(inspector_float_delegate cb);

			connection_id subscribe_inspector_draw_gui(inspector_void_delegate cb);
			connection_id subscribe_reload_images(inspector_void_delegate cb);
			connection_id subscribe_reload_shaders(inspector_void_delegate cb);
			
			connection_id subscribe_set_render_mode(inspector_render_mode_delegate cb);
			connection_id subscribe_set_view_direction(inspector_view_delegate cb);
			connection_id subscribe_set_camera_control_mode(inspector_camera_control_delegate cb);
			connection_id subscribe_cook_and_package(inspector_cook_and_package_delegate cb);

			//-------------------------------------------------------------------------
            /**
			 * @brief Emit functions for engine events
			 */
			void emit_pause(bool v);
			void emit_unpause(bool v);
			void emit_next_frame(bool v);
			void emit_set_show_static(bool v);
			void emit_set_show_dynamic(bool v);
			void emit_set_show_bounding_boxes(bool v);
			void emit_set_show_grid(bool v);
			void emit_set_camera_invert_y(bool v);

			void emit_set_orbit_camera_pan_speed(float v);
			void emit_set_orbit_camera_zoom_speed(float v);
			void emit_set_orbit_camera_rotation_speed(float v);
			void emit_set_orbit_camera_min_zoom(float v);
			void emit_set_orbit_camera_max_zoom(float v);
			void emit_set_free_camera_look_speed(float v);
			void emit_set_free_camera_move_speed(float v);
			void emit_set_free_camera_boost_multiplier(float v);
			void emit_set_fov(float v);

			void emit_inspector_draw_gui();
			void emit_reload_images();
			void emit_reload_shaders();

			void emit_set_render_mode(render_mode rm);
			void emit_set_view_direction(view_direction vd);
			void emit_set_camera_control_mode(camera_control_type cct);
			void emit_cook_and_package(const package_info& info);

			//-------------------------------------------------------------------------
			/**
			 * @brief Unsubscription function for engine events
			 */
            bool unsubscribe_inspector_signal(connection_id id);
        }
    }
}