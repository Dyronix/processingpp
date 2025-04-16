#pragma once

#include "vector.h"

#include <string>

namespace ppp
{
    namespace camera
    {
        namespace tags
        {
            std::string_view perspective();
            std::string_view orthographic();
            std::string_view font();
        }
    }

    struct scene_camera
    {
        scene_camera();

        vec3 eye;
        vec3 center;
        vec3 up;

        float radius = 0.0f;   // Distance from camera to target
        float azimuth = 0.0f;  // Horizontal angle
        float polar = 0.0f;    // Vertical angle (default at 45 degrees)

        void set_position(float x, float y, float z);
        void set_center(float centerx, float centery, float centerz);
        void set_up_direction(float upx, float upy, float upz);
    };

    struct orbit_control_options
    {
        float zoom_sensitivity = 0.0f;
        float rotation_sensitivity = 0.0f;
        float panning_sensitivity = 0.0f;

        float min_zoom = 1.0f;
        float max_zoom = 100.0f;
    };

    vec3 active_camera_position();
    vec3 active_camera_target();
    vec3 active_camera_up();

    scene_camera create_camera(std::string_view camera_tag = {});

    void set_scene_camera(scene_camera c, std::string_view camera_tag = {});
    void set_scene_camera(float eyex, float eyey, float eyez = 800.0f, float centerx = 0.0f, float centery = 0.0f, float centerz = 0.0f, float upx = 0.0f, float upy = 1.0f, float upz = 0.0f, std::string_view camera_tag = {});

    void activate_camera(std::string_view camera_tag);

    void ortho(float left, float right, float bottom, float top, float near, float far, std::string_view camera_tag = {});
    void perspective(float fovy, float aspect, float near, float far, std::string_view camera_tag = {});

    void orbit_control(orbit_control_options options, std::string_view camera_tag = {});
}