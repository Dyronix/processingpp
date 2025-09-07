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
		}
	}
}
