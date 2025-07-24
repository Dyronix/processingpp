#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{	
	enum class key_code;

	namespace imgui
	{
		namespace inspector
		{
			/**
			 * @brief Inspector notification types
			 */
			enum class notification_type : std::uint8_t
			{
				INFO,
				SUCCESS,
				WARNING,
				ERROR
			};

			/**
			 * @brief C-style function pointer type for inspector event callbacks.
			 */
			using inspector_delegate = void(*)(bool value);
			using inspector_reload_images_delegate = void(*)();

			/**
			 * @brief Subscription functions for engine events
			 */
			void subscribe_pause(inspector_delegate callback);
			void subscribe_unpause(inspector_delegate callback);
			void subscribe_next_frame(inspector_delegate callback);
			void subscribe_reload_images(inspector_reload_images_delegate callback);

			/**
			 * @brief Shortcuts for inspector controls
			 */
			void set_pause_shortcut(key_code code);
			void set_nextframe_shortcut(key_code code);

			/**
			 * @brief Inspector functionality
			 */
			void initialize();
			void shutdown();
			void render(f32 dt);

			int notify(notification_type type, const std::string& message, f32 time);		
		}
	}
}
