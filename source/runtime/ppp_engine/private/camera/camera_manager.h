#pragma once

#include "util/types.h"



#include "string/string_id.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace camera_manager
    {
        namespace tags
        {
            string::string_id perspective();
            string::string_id orthographic();
            string::string_id font();
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

        camera* set_camera(string::string_id camera_tag, const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj);
        camera* set_as_active_camera(string::string_id camera_tag);

        const glm::vec3& get_camera_position();
        const glm::vec3& get_camera_position(string::string_id camera_tag);
        const glm::vec3& get_camera_lookat();
        const glm::vec3& get_camera_lookat(string::string_id camera_tag);

        const glm::mat4& get_view();
        const glm::mat4& get_view(string::string_id camera_tag);
        const glm::mat4& get_proj();
        const glm::mat4& get_proj(string::string_id camera_tag);

        camera* camera_by_tag(string::string_id camera_tag);
    }
}
