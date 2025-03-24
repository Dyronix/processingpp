#include "camera.h"
#include "camera/camera_manager.h"
#include "events.h"
#include "environment.h"
#include "camera/camera_manager.h"
#include "device/device.h"

#include "util/types.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ppp
{
    namespace camera
    {
        namespace tags
        {
            //-------------------------------------------------------------------------
            std::string_view perspective()
            {
                return string::restore_sid(camera_manager::tags::perspective());
            }

            //-------------------------------------------------------------------------
            std::string_view orthographic()
            {
                return string::restore_sid(camera_manager::tags::orthographic());
            }

            //-------------------------------------------------------------------------
            std::string_view font()
            {
                return string::restore_sid(camera_manager::tags::font());
            }
        }
    }

    namespace internal
    {
        // Constants for sensitivity
        const float _rotate_sensitivity = 0.05f;
        const float _zoom_sensitivity = 1.0f;
        const float _pan_ensitivity = 0.05f;

        struct ortho
        {
            float left, right, bottom, top = -1.0f;
            float near, far = -1.0f;
        };

        struct perspective
        {
            float fovy, aspect = -1.0f;
            float near, far = -1.0f;
        };

        ortho       _active_ortho = {};
        perspective _active_perspective = {};
        glm::mat4   _active_projection = glm::mat4(1.0f);
        bool        _active_projection_is_ortho = true;

        scene_camera _active_camera = {};

        //-------------------------------------------------------------------------
        void push_active_camera(std::string_view camera_tag)
        {
            glm::vec3 eye = { internal::_active_camera.eye.x, internal::_active_camera.eye.y, internal::_active_camera.eye.z };
            glm::vec3 center = { internal::_active_camera.center.x, internal::_active_camera.center.y, internal::_active_camera.center.z };
            glm::vec3 up = { internal::_active_camera.up.x, internal::_active_camera.up.y, internal::_active_camera.up.z };

            string::string_id sid = string::string_id(camera_tag);

            camera_manager::set_camera(sid, eye, center, up, _active_projection);
            camera_manager::set_as_active_camera(sid);
        }
    }

    //-------------------------------------------------------------------------
    scene_camera::scene_camera()
    {
        eye.x = 0.0f;
        eye.y = 0.0f;
        eye.z = 100.0f;

        center.x = 0.0f;
        center.y = 0.0f;
        center.z = 0.0f;

        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;

        radius = glm::length(glm::vec3(eye.x, eye.y, eye.z) - glm::vec3(center.x, center.y, center.z));
        azimuth = 0.0f;
        polar = glm::radians(45.0f);
    }

    //-------------------------------------------------------------------------
    void scene_camera::set_position(float in_eyex, float in_eyey, float in_eyez)
    {
        eye.x = in_eyex;
        eye.y = in_eyey;
        eye.z = in_eyez;

        radius = glm::length(glm::vec3(eye.x, eye.y, eye.z) - glm::vec3(center.x, center.y, center.z));
    }
    //-------------------------------------------------------------------------
    void scene_camera::set_up_direction(float in_upx, float in_upy, float in_upz)
    {
        up.x = in_upx;
        up.y = in_upy;
        up.z = in_upz;
    }
    //-------------------------------------------------------------------------
    void scene_camera::set_center(float in_centerx, float in_centery, float in_centerz)
    {
        center.x = in_centerx;
        center.y = in_centery;
        center.z = in_centerz;

        radius = glm::length(glm::vec3(eye.x, eye.y, eye.z) - glm::vec3(center.x, center.y, center.z));
    }

    //-------------------------------------------------------------------------
    vec3 active_camera_position()
    {
        return { internal::_active_camera.eye.x, internal::_active_camera.eye.y, internal::_active_camera.eye.z };
    }
    //-------------------------------------------------------------------------
    vec3 active_camera_target()
    {
        return { internal::_active_camera.center.x, internal::_active_camera.center.y, internal::_active_camera.center.z };
    }
    //-------------------------------------------------------------------------
    vec3 active_camera_up()
    {
        return { internal::_active_camera.up.x, internal::_active_camera.up.y, internal::_active_camera.up.z };
    }

    //-------------------------------------------------------------------------
    scene_camera create_camera(std::string_view camera_tag)
    {
        scene_camera c = scene_camera();

        set_scene_camera(c, camera_tag);

        return internal::_active_camera;
    }

    //-------------------------------------------------------------------------
    void set_scene_camera(scene_camera c, std::string_view camera_tag)
    {
        set_scene_camera(c.eye.x, c.eye.y, c.eye.z, c.center.x, c.center.y, c.center.z, c.up.x, c.up.y, c.up.z, camera_tag);
    }
    //-------------------------------------------------------------------------
    void set_scene_camera(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz, std::string_view camera_tag)
    {
        internal::_active_camera.set_position(eyex, eyey, eyez);
        internal::_active_camera.set_center(centerx, centery, centerz);
        internal::_active_camera.set_up_direction(upx, upy, upz);

        f32 width = canvas_width();
        f32 height = canvas_height();

        if (internal::_active_projection_is_ortho)
        {
            f32 half_width = width * 0.5f;
            f32 half_height = height * 0.5f;

            f32 left = internal::_active_ortho.left == -1.0f ? -half_width : internal::_active_ortho.left;
            f32 right = internal::_active_ortho.right == -1.0f ? +half_width : internal::_active_ortho.right;
            f32 bottom = internal::_active_ortho.bottom == -1.0f ? +half_height : internal::_active_ortho.bottom;
            f32 top = internal::_active_ortho.top == -1.0f ? -half_height : internal::_active_ortho.top;

            f32 near = internal::_active_ortho.near == -1.0f ? 0.0f : internal::_active_ortho.near;
            f32 far = internal::_active_ortho.far == -1.0f ? std::max(width, height) + 800.0f : internal::_active_ortho.far;

            ortho(left, right, bottom, top, near, far, camera_tag); // will push the active camera
        }
        else
        {
            f32 fovy = internal::_active_perspective.fovy == -1.0f ? 2 * atan((height * 0.5f) / 800.0f) : internal::_active_perspective.fovy;
            f32 aspect = internal::_active_perspective.aspect == -1.0f ? width / height : internal::_active_perspective.aspect;
            f32 near = internal::_active_perspective.near == -1.0f ? 0.1f * 800.0f : internal::_active_perspective.near;
            f32 far = internal::_active_perspective.far == -1.0f ? 10.0f * 800.0f : internal::_active_perspective.far;

            perspective(fovy, aspect, near, far, camera_tag); // will push the active camera
        }
    }

    //-------------------------------------------------------------------------
    void activate_camera(std::string_view camera_tag)
    {
        string::string_id sid = string::string_id(camera_tag);

        camera_manager::camera* c = camera_manager::camera_by_tag(sid);
        if (c)
        {
            camera_manager::set_as_active_camera(sid);

            internal::_active_camera.eye.x = c->eye.x; internal::_active_camera.eye.y = c->eye.y; internal::_active_camera.eye.z = c->eye.z;
            internal::_active_camera.center.x = c->target.x; internal::_active_camera.center.y = c->target.y; internal::_active_camera.center.z = c->target.z;
            internal::_active_camera.up.x = c->up.x; internal::_active_camera.up.y = c->up.y; internal::_active_camera.up.z = c->up.z;
        }
    }

    //-------------------------------------------------------------------------
    void ortho(float left, float right, float bottom, float top, float near, float far, std::string_view camera_tag)
    {
        internal::_active_ortho.left = left;
        internal::_active_ortho.right = right;
        internal::_active_ortho.bottom = bottom;
        internal::_active_ortho.top = top;
        internal::_active_ortho.near = near;
        internal::_active_ortho.far = far;

        internal::_active_projection_is_ortho = true;
        internal::_active_projection = glm::ortho(
            left,
            right,
            bottom,
            top,
            near,
            far);

        internal::push_active_camera(camera_tag.empty() ? camera::tags::orthographic() : camera_tag);
    }
    //-------------------------------------------------------------------------
    void perspective(float fovy, float aspect, float near, float far, std::string_view camera_tag)
    {
        internal::_active_perspective.fovy = fovy;
        internal::_active_perspective.aspect = aspect;
        internal::_active_perspective.near = near;
        internal::_active_perspective.far = far;

        internal::_active_projection_is_ortho = false;
        internal::_active_projection = glm::perspective(
            glm::radians(fovy),
            aspect,
            near,
            far);

        internal::push_active_camera(camera_tag.empty() ? camera::tags::perspective() : camera_tag);
    }

    //-------------------------------------------------------------------------
    void orbit_control(orbit_control_options options, std::string_view camera_tag)
    {
        f32 zs = options.zoom_sensitivity != 0.0f ? options.zoom_sensitivity : internal::_zoom_sensitivity;
        f32 rs = options.rotation_sensitivity != 0.0f ? options.rotation_sensitivity : internal::_rotate_sensitivity;
        f32 ps = options.panning_sensitivity != 0.0f ? options.panning_sensitivity : internal::_pan_ensitivity;

        f32 delta_x = moved_x();
        f32 delta_y = moved_y();

        f32 scroll = scroll_offset_y();

        if (is_left_button_pressed())
        {
            internal::_active_camera.azimuth -= delta_x * rs * device::delta_time(); // Horizontal rotation (Azimuth)
            internal::_active_camera.polar -= delta_y * rs * device::delta_time();   // Vertical rotation (Polar)

            // Clamp polar angle to avoid flipping the camera
            internal::_active_camera.polar = glm::clamp(internal::_active_camera.polar, 0.1f, glm::pi<float>() - 0.1f);
        }

        if (is_right_button_pressed())
        {
            glm::vec3 cam_eye = { internal::_active_camera.eye.x,internal::_active_camera.eye.y,internal::_active_camera.eye.z };
            glm::vec3 cam_target = { internal::_active_camera.center.x,internal::_active_camera.center.y,internal::_active_camera.center.z };
            glm::vec3 cam_up = { internal::_active_camera.up.x,internal::_active_camera.up.y,internal::_active_camera.up.z };

            glm::vec3 right = glm::normalize(glm::cross(cam_target - cam_eye, cam_up)); // Right vector3
            glm::vec3 up = glm::normalize(up);                              // Up vector3

            glm::vec3 pan_right = right * delta_x * ps * internal::_active_camera.radius * device::delta_time();
            glm::vec3 pan_up = up * delta_y * ps * internal::_active_camera.radius * device::delta_time();

            // Shift target position for panning
            cam_target += -pan_right + pan_up;

            internal::_active_camera.center.x = cam_target.x;
            internal::_active_camera.center.y = cam_target.y;
            internal::_active_camera.center.z = cam_target.z;
        }

        if (scroll != 0.0f)
        {
            internal::_active_camera.radius -= scroll * zs * device::delta_time();

            // Clamp radius to avoid zooming too close or too far
            internal::_active_camera.radius = glm::clamp(internal::_active_camera.radius, options.min_zoom, options.max_zoom);
        }

        internal::_active_camera.eye.x = internal::_active_camera.center.x + internal::_active_camera.radius * sin(internal::_active_camera.polar) * sin(internal::_active_camera.azimuth);
        internal::_active_camera.eye.y = internal::_active_camera.center.y + internal::_active_camera.radius * cos(internal::_active_camera.polar);  // Height
        internal::_active_camera.eye.z = internal::_active_camera.center.z + internal::_active_camera.radius * sin(internal::_active_camera.polar) * cos(internal::_active_camera.azimuth);

        internal::push_active_camera(camera_tag);
    }
}