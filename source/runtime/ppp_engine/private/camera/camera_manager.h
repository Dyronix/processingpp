#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace camera_manager
    {
        namespace tags
        {
            static const std::string perspective = "perspective";
            static const std::string orthographic = "orthographic";
            static const std::string font = "font";
        }

        struct camera
        {
            glm::vec3 eye = glm::vec3(0.0f, 0.0f, 1.0f);
            glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::mat4 proj = glm::mat4(1.0f);
        };

        bool initialize(f32 frustum_width, f32 frustum_height);
        void terminate();

        camera* set_camera(std::string_view camera_tag, const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj);
        camera* set_as_active_camera(std::string_view camera_tag);

        const glm::mat4& get_view();
        const glm::mat4& get_view(std::string_view camera_tag);
        const glm::mat4& get_proj();
        const glm::mat4& get_proj(std::string_view camera_tag);

        camera* camera_by_tag(std::string_view camera_tag);
    }
}
