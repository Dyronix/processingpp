/**
 * @file camera.h
 * @brief Create and control scene cameras (perspective, orthographic, font).
 */

#pragma once

#include "vector.h"
#include "ray.h"

#include <string>

namespace ppp
{
    namespace camera
    {
        namespace tags
        {
            /**
             * @brief Retrieve the identifier for the perspective camera.
             * @return String tag for perspective camera.
             */
            std::string_view perspective();

            /**
             * @brief Retrieve the identifier for the orthographic camera.
             * @return String tag for orthographic camera.
             */
            std::string_view orthographic();

            /**
             * @brief Retrieve the identifier for the ui-rendering camera.
             * @return String tag for ui camera.
             */
            std::string_view ui();

            /**
             * @brief Retrieve the identifier for the font-rendering camera.
             * @return String tag for font camera.
             */
            std::string_view font();
        }
    }

    /**
     * @brief Represents a scene camera with position, orientation, and target.
     */
    struct scene_camera
    {
        /** @brief Default constructor initializing common defaults. */
        scene_camera();

        /** @brief Camera position in world space. */
        vec3 eye;
        /** @brief Target point the camera looks at. */
        vec3 center;
        /** @brief Up vector defining camera orientation. */
        vec3 up;       

        /** @brief Distance from eye to center. */
        float radius;
        /** @brief Horizontal rotation angle around target. */
        float azimuth;
        /** @brief Vertical rotation angle around target. */
        float polar;   

        /**
         * @brief Set the camera's eye position.
         * @param x X-coordinate of eye.
         * @param y Y-coordinate of eye.
         * @param z Z-coordinate of eye.
         */
        void set_position(float x, float y, float z);

        /**
         * @brief Set the camera's target point.
         * @param cx X-coordinate of center.
         * @param cy Y-coordinate of center.
         * @param cz Z-coordinate of center.
         */
        void set_center(float cx, float cy, float cz);

        /**
         * @brief Set the camera's up direction.
         * @param ux X-component of up vector.
         * @param uy Y-component of up vector.
         * @param uz Z-component of up vector.
         */
        void set_up_direction(float ux, float uy, float uz);
    };

    /**
     * @brief Options for interactive orbit control.
     */
    struct orbit_control_options
    {
        /** @brief Mouse zoom sensitivity. */
        float zoom_sensitivity = 0.0f;
        /** @brief Mouse rotation sensitivity. */
        float rotation_sensitivity = 0.0f;
        /** @brief Mouse panning sensitivity. */
        float panning_sensitivity = 0.0f;

        /** @brief Minimum allowed zoom distance. */
        float min_zoom = 1.0f;
        /** @brief Maximum allowed zoom distance. */
        float max_zoom = 100.0f;
    };

    /**
     * @brief Options for interactive free-flight control.
     */
    struct free_control_options
    {
        /** @brief Mouse look sensitivity. */
        float look_sensitivity = 0.01f;
        /** @brief Movement speed in units per second. */
        float movement_speed = 100.0f;
        /** @brief Speed multiplier when shift is held. */
        float boost_multiplier = 4.0f;
    };


    /**
      * @brief Get the active camera's eye position.
      * @return vec3 Eye position of active camera.
      */
    vec3 active_camera_position();

    /**
     * @brief Get the active camera's target point.
     * @return vec3 Look-at center of active camera.
     */
    vec3 active_camera_target();

    /**
     * @brief Get the active camera's up vector.
     * @return vec3 Up direction of active camera.
     */
    vec3 active_camera_up();

    /**
     * @brief Create and activate a scene camera with optional tag.
     * @param tag Optional identifier for the new camera.
     * @return Initialized scene_camera object.
     */
    scene_camera create_camera(std::string_view tag = {});

    /**
     * @brief Set the active camera using a scene_camera object.
     * @param c Scene camera instance.
     * @param tag Optional identifier for the camera.
     */
    void set_scene_camera(scene_camera c, std::string_view tag = {});

    /**
     * @brief Set the active camera by specifying parameters directly.
     * @param ex Eye X-coordinate.
     * @param ey Eye Y-coordinate.
     * @param ez Eye Z-coordinate.
     * @param cx Center X-coordinate.
     * @param cy Center Y-coordinate.
     * @param cz Center Z-coordinate.
     * @param ux Up X-component.
     * @param uy Up Y-component.
     * @param uz Up Z-component.
     * @param tag Optional identifier for the camera.
     */
    void set_scene_camera(
                        float ex, float ey, float ez = 800.0f,
                        float cx = 0.0f, float cy = 0.0f, float cz = 0.0f,
                        float ux = 0.0f, float uy = 1.0f, float uz = 0.0f,
                        std::string_view tag = {});

    /**
     * @brief Activate a previously created camera by its tag.
     * @param tag Identifier of the camera to activate.
     */
    void activate_camera(std::string_view tag);

    /**
     * @brief Configure and activate an orthographic projection.
     * @param left Left plane of orthographic frustum.
     * @param right Right plane of orthographic frustum.
     * @param bottom Bottom plane of orthographic frustum.
     * @param top Top plane of orthographic frustum.
     * @param near Near clipping plane.
     * @param far Far clipping plane.
     * @param tag Optional camera identifier.
     */
    void ortho(
            float left, float right,
            float bottom, float top, 
            float near, float far,
            std::string_view tag = {});

    /**
     * @brief Configure and activate a perspective projection.
     * @param fovy Vertical field of view in degrees.
     * @param aspect Aspect ratio (width/height).
     * @param near Near clipping plane distance.
     * @param far Far clipping plane distance.
     * @param tag Optional camera identifier.
     */
    void perspective(
                float fovy, float aspect,
                float near, float far,
                std::string_view tag = {});

    /**
     * @brief Enable interactive orbit controls for the camera.
     * @param opts Orbit control sensitivity and zoom limits.
     * @param tag Identifier for the camera to control.
     */
    void orbit_control(orbit_control_options opts, std::string_view tag = {});

    /**
     * @brief Enable free-flight controls for the camera.
     * @param opts Free control sensitivity and speed settings.
     * @param camera_tag Identifier for the camera to control.
     */
    void free_control(free_control_options opts, std::string_view camera_tag = {});

    /**
     * @brief Generate a world-space ray from screen-space coordinates.
     * @param screen_x X position in screen space (e.g. mouse X).
     * @param screen_y Y position in screen space (e.g. mouse Y).
     * @param screen_width Width of the render surface.
     * @param screen_height Height of the render surface.
     * @return A pair of vectors: ray origin and ray direction in world space.
     */
    ray screen_to_world(float screen_x, float screen_y, float screen_width, float screen_height);
}