#include "camera.h"
#include "camera/camera_manager.h"
#include "events.h"
#include "environment.h"
#include "device/device.h"

#include "util/log.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.inl>
#include <glm/gtx/compatibility.inl>

#include <algorithm>
#include <unordered_map>

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
            std::string_view ui()
            {
                return string::restore_sid(camera_manager::tags::ui());
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

        // Camera modes
        enum class camera_mode
        {
            ORBIT,
            FREE
        };

        // Stored camera state
        struct camera_state
        {
            camera_mode mode = camera_mode::ORBIT;
            scene_camera state = {};
        };

        // Projection parameter structs
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

        // Current active projection parameters
        ortho           _active_ortho = {};
        perspective     _active_perspective = {};
        glm::mat4       _active_projection = glm::mat4(1.0f);
        bool            _active_projection_is_ortho = true;

        scene_camera    _active_camera = {};

        // Last used camera mode
        using camera_states = std::unordered_map<camera_mode, camera_state>;

        camera_mode     _last_mode = camera_mode::ORBIT;
        camera_states   _camera_states = {};

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
        // --- Mode transition detection (Free -> Orbit) ---
        if (internal::_last_mode != internal::camera_mode::ORBIT)
        {
            // Store the FREE camera state
            internal::_camera_states[internal::camera_mode::FREE] = { internal::camera_mode::FREE, internal::_active_camera };

            // Load the ORBIT camera state if it exists
            if(internal::_camera_states.find(internal::camera_mode::ORBIT) != internal::_camera_states.end())
            {
                internal::_active_camera = internal::_camera_states[internal::camera_mode::ORBIT].state;
            }

            // Bookkeeping of the last mode
            internal::_last_mode = internal::camera_mode::ORBIT;
        }
            
        f32 zs = options.zoom_sensitivity != 0.0f ? options.zoom_sensitivity : internal::_zoom_sensitivity;
        f32 rs = options.rotation_sensitivity != 0.0f ? options.rotation_sensitivity : internal::_rotate_sensitivity;
        f32 ps = options.panning_sensitivity != 0.0f ? options.panning_sensitivity : internal::_pan_ensitivity;

        f32 delta_x = moved_x();
        f32 delta_y = moved_y();

        f32 scroll = scroll_offset_y();

        if (is_left_button_pressed())
        {
            float dt = glm::clamp(device::delta_time(), 0.0f, 0.016f); // avoid huge jumps on hitches (>~60 FPS frame)

            internal::_active_camera.azimuth -= delta_x * rs * dt; // Horizontal rotation (Azimuth)
            internal::_active_camera.polar -= delta_y * rs * dt;   // Vertical rotation (Polar)

            // Clamp polar angle to avoid flipping the camera
            internal::_active_camera.polar = glm::clamp(internal::_active_camera.polar, 0.1f, glm::pi<float>() - 0.1f);
        }

        if (is_right_button_pressed())
        {
            glm::vec3 cam_eye = { internal::_active_camera.eye.x,internal::_active_camera.eye.y,internal::_active_camera.eye.z };
            glm::vec3 cam_target = { internal::_active_camera.center.x,internal::_active_camera.center.y,internal::_active_camera.center.z };
            glm::vec3 cam_up = { internal::_active_camera.up.x,internal::_active_camera.up.y,internal::_active_camera.up.z };

            // Camera basis
            glm::vec3 view = glm::normalize(cam_target - cam_eye);          // forward
            glm::vec3 right = glm::normalize(glm::cross(view, cam_up));     // right
            glm::vec3 up = glm::normalize(glm::cross(right, view));         // re-orthogonalized "up"

            // --- Speed control ---
            float dt = glm::clamp(device::delta_time(), 0.0f, 0.016f); // avoid huge jumps on hitches (>~60 FPS frame)
            // NOTE: most windowing systems have +delta_y when moving mouse *down*.
            // If your Y is inverted, flip the sign below.
            float pan_scale = ps * internal::_active_camera.radius * dt; // tune ps; typically don't multiply by dt for mouse deltas

            glm::vec3 pan = (-delta_x * right + -delta_y * up) * pan_scale;

            // Pan moves the whole rig: eye and target together
            cam_eye += pan;
            cam_target += pan;

            internal::_active_camera.eye.x = cam_eye.x;
            internal::_active_camera.eye.y = cam_eye.y;
            internal::_active_camera.eye.z = cam_eye.z;

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

    //-------------------------------------------------------------------------
    void free_control(free_control_options options, std::string_view camera_tag)
    {
        // --- Mode transition detection (Orbit -> Free) ---
        if (internal::_last_mode != internal::camera_mode::FREE)
        {
            // Store the ORBIT camera state
            internal::_camera_states[internal::camera_mode::ORBIT] = { internal::camera_mode::ORBIT, internal::_active_camera };

            // Load the FREE camera state if it exists
            if (internal::_camera_states.find(internal::camera_mode::FREE) != internal::_camera_states.end())
            {
                internal::_active_camera = internal::_camera_states[internal::camera_mode::FREE].state;
            }

            // Bookkeeping of the last mode
            internal::_last_mode = internal::camera_mode::FREE;
        }

        // --- Gather current camera state into GLM ---
        glm::vec3 eye = { internal::_active_camera.eye.x,    internal::_active_camera.eye.y,    internal::_active_camera.eye.z };
        glm::vec3 target = { internal::_active_camera.center.x, internal::_active_camera.center.y, internal::_active_camera.center.z };
        glm::vec3 up = { internal::_active_camera.up.x,     internal::_active_camera.up.y,     internal::_active_camera.up.z };

        // Derive basis
        glm::vec3 front = glm::normalize(target - eye);
        if (!glm::all(glm::isfinite(front))) { front = glm::vec3(0, 0, -1); }

        glm::vec3 right = glm::normalize(glm::cross(front, up));
        if (!glm::all(glm::isfinite(right))) { right = glm::vec3(1, 0, 0); }

        up = glm::normalize(glm::cross(right, front)); // re-orthonormalize up

        // --- Mouse look when RMB is held ---
        if (is_right_button_pressed())
        {
            float dx = moved_x();
            float dy = moved_y();

            // Recover yaw/pitch from current 'front'
            float yaw = std::atan2(front.x, front.z);     // left/right
            float pitch = std::asin(glm::clamp(front.y, -1.0f, 1.0f)); // up/down

            // Apply mouse deltas (no dt on mouse)
            float sens = (options.look_sensitivity != 0.0f) ? options.look_sensitivity : 0.1f;
            yaw -= dx * sens;
            pitch += dy * sens;

            // Clamp pitch to avoid gimbal flip
            constexpr float c_max_pitch = glm::radians(89.0f);
            pitch = glm::clamp(pitch, -c_max_pitch, c_max_pitch);

            // Rebuild front from yaw/pitch
            front = glm::normalize(glm::vec3(std::sin(yaw) * std::cos(pitch),std::sin(pitch),std::cos(yaw) * std::cos(pitch)));

            right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
            up = glm::normalize(glm::cross(right, front));
        }
        // --- Keyboard movement when RMB is held ---
        if (is_right_button_pressed())
        {
            f32 dt = glm::clamp(device::delta_time(), 0.0f, 0.016f);
            f32 speed = options.movement_speed * dt;

            if (is_key_down(key_code::KEY_LEFT_SHIFT)) { speed *= options.boost_multiplier; }

            if (is_key_down(key_code::KEY_W)) { eye += speed * front; }
            if (is_key_down(key_code::KEY_S)) { eye -= speed * front; }
            if (is_key_down(key_code::KEY_A)) { eye -= speed * right; }
            if (is_key_down(key_code::KEY_D)) { eye += speed * right; }
            if (is_key_down(key_code::KEY_Q)) { eye += speed * up; }
            if (is_key_down(key_code::KEY_E)) { eye -= speed * up; }
        }

        // Keep looking forward
        target = eye + front;

        // --- Write back to scene_camera ---
        internal::_active_camera.eye.x = eye.x;    internal::_active_camera.eye.y = eye.y;    internal::_active_camera.eye.z = eye.z;
        internal::_active_camera.center.x = target.x; internal::_active_camera.center.y = target.y; internal::_active_camera.center.z = target.z;
        internal::_active_camera.up.x = up.x;     internal::_active_camera.up.y = up.y;     internal::_active_camera.up.z = up.z;

        internal::push_active_camera(camera_tag);
    }

    //-------------------------------------------------------------------------
    ray screen_to_world(float screen_x, float screen_y, float screen_width, float screen_height)
    {
        const glm::mat4& proj = camera_manager::get_proj();
        const glm::mat4& view = camera_manager::get_view();

        float x_ndc = (2.0f * screen_x) / screen_width - 1.0f;
        float y_ndc = (2.0f * screen_y) / screen_height - 1.0f;
        glm::vec2 ndc = glm::vec2(x_ndc, y_ndc);

        glm::vec4 ray_clip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f); 

        glm::mat4 inv_proj = glm::inverse(proj);
        glm::vec4 ray_eye = inv_proj * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        glm::mat4 inv_view = glm::inverse(view);
        glm::vec4 ray_world = inv_view * ray_eye;
        glm::vec3 ray_dir = glm::normalize(glm::vec3(ray_world));
        glm::vec3 cam_pos = glm::vec3(inv_view[3]);

        return { cam_pos , ray_dir };
    }

}