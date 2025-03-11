#pragma once

namespace ppp
{
    namespace render
    {
        struct render_context
        {
            glm::vec3 camera_position_font;
            glm::vec3 camera_lookat_font;

            glm::mat4 mat_view_font;
            glm::mat4 mat_proj_font;

            glm::vec3 camera_position_active;
            glm::vec3 camera_lookat_active;

            glm::mat4 mat_view_active;
            glm::mat4 mat_proj_active;
        };
    }
}