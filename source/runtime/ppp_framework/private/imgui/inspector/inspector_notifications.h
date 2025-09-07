#pragma once

#include "util/types.h"
#include <string>

namespace ppp
{
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
             * @brief Push a notification to be displayed.
             * @param type The notification type (icon/color).
             * @param message The message to show.
             * @param time Seconds to keep the toast visible.
             * @return A unique id for this notification.
             */
            s32 notify(notification_type type, const std::string& message, f32 time);

            /**
             * @brief Render and update all active notifications.
             * Call once per frame, typically from the main inspector render loop.
             */
            void render_notifications(f32 dt);

            /**
             * @brief Remove all active notifications.
             */
            void clear_notifications();
        }
    }
}
