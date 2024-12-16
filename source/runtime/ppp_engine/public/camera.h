#pragma once

#include <glm/glm.hpp>

namespace ppp
{
    namespace camera
    {
        enum class scene_camera_mode
        {
            CAMERA_3D,
            CAMERA_2D,
            CAMERA_IMAGE,
            CAMERA_FONT
        };

        struct scene_camera
        {
            scene_camera();

            float eyex, eyey, eyez;
            float centerx, centery, centerz;
            float upx, upy, upz;

            float radius;   // Distance from camera to target
            float azimuth;  // Horizontal angle
            float polar;    // Vertical angle (default at 45 degrees)

            void set_position(float x, float y, float z);
            void set_center(float centerx, float centery, float centerz);
            void set_up_direction(float upx, float upy, float upz);
        };

        struct orbit_scene_camera_options
        {
            float zoom_sensitivity = 0.0f;
            float rotation_sensitivity = 0.0f;
            float panning_sensitivity = 0.0f;

            float min_zoom = 1.0f;
            float max_zoom = 100.0f;
        };

        glm::vec3 active_camera_position();
        glm::vec3 active_camera_target();
        glm::vec3 active_camera_up();

        scene_camera create_camera();

        void set_scene_camera(scene_camera c);
        void set_scene_camera(float eyex, float eyey, float eyez = 800.0f, float centerx = 0.0f, float centery = 0.0f, float centerz = 0.0f, float upx = 0.0f, float upy = 1.0f, float upz = 0.0f);

        void set_scene_camera_mode(scene_camera_mode m);

        void ortho(float left, float right, float bottom, float top, float near, float far);
        void perspective(float fovy, float aspect, float near, float far);

        void orbit_control(orbit_scene_camera_options options = {});
    }
}