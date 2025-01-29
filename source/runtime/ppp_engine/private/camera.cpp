#include "camera.h"
#include "environment.h"
#include "events.h"

#include "device/device.h"

#include "render/render.h"

#include "camera/camera_manager.h"

#include "util/types.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ppp
{
    namespace camera
    {
        namespace internal
        {
            using scene_camera_tag = std::string;

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
            void push_active_camera(const std::string& camera_tag)
            {
                glm::vec3 eye = { internal::_active_camera.eyex, internal::_active_camera.eyey, internal::_active_camera.eyez };
                glm::vec3 center = { internal::_active_camera.centerx, internal::_active_camera.centery, internal::_active_camera.centerz };
                glm::vec3 up = { internal::_active_camera.upx, internal::_active_camera.upy, internal::_active_camera.upz };

                camera_manager::set_camera(camera_tag, eye, center, up, _active_projection);
                camera_manager::set_as_active_camera(camera_tag);
            }
        }

        namespace tags
        {
            //-------------------------------------------------------------------------
            const std::string& perspective()
            {
                return camera_manager::tags::perspective;
            }

            //-------------------------------------------------------------------------
            const std::string& orthographic()
            {
                return camera_manager::tags::orthographic;
            }

            //-------------------------------------------------------------------------
            const std::string& font()
            {
                return camera_manager::tags::font;
            }
        }

        //-------------------------------------------------------------------------
        scene_camera::scene_camera()
        {
            eyex = 0.0f;
            eyey = 0.0f;
            eyez = 100.0f;

            centerx = 0.0f;
            centery = 0.0f;
            centerz = 0.0f;

            upx = 0.0f;
            upy = 1.0f;
            upz = 0.0f;

            radius = glm::length(glm::vec3(eyex, eyey, eyez) - glm::vec3(centerx, centery, centerz));
            azimuth = 0.0f;
            polar = glm::radians(45.0f);
        }

        //-------------------------------------------------------------------------
        void scene_camera::set_position(float in_eyex, float in_eyey, float in_eyez)
        {
            eyex = in_eyex;
            eyey = in_eyey; 
            eyez = in_eyez;

            radius = glm::length(glm::vec3(eyex, eyey, eyez) - glm::vec3(centerx, centery, centerz));
        }
        //-------------------------------------------------------------------------
        void scene_camera::set_up_direction(float in_upx, float in_upy, float in_upz)
        {
            upx = in_upx;
            upy = in_upy;
            upz = in_upz;
        }
        //-------------------------------------------------------------------------
        void scene_camera::set_center(float in_centerx, float in_centery, float in_centerz)
        {
            centerx = in_centerx;
            centery = in_centery;
            centerz = in_centerz;

            radius = glm::length(glm::vec3(eyex, eyey, eyez) - glm::vec3(centerx, centery, centerz));
        }

        //-------------------------------------------------------------------------
        glm::vec3 active_camera_position()
        {
            return { internal::_active_camera.eyex, internal::_active_camera.eyey, internal::_active_camera.eyez };
        }
        //-------------------------------------------------------------------------
        glm::vec3 active_camera_target()
        {
            return { internal::_active_camera.centerx, internal::_active_camera.centery, internal::_active_camera.centerz };
        }
        //-------------------------------------------------------------------------
        glm::vec3 active_camera_up()
        {
            return { internal::_active_camera.upx, internal::_active_camera.upy, internal::_active_camera.upz };
        }

        //-------------------------------------------------------------------------
        scene_camera create_camera(const std::string& camera_tag)
        {
            scene_camera c = scene_camera();

            set_scene_camera(c, camera_tag);

            return internal::_active_camera;
        }
        
        //-------------------------------------------------------------------------
        void set_scene_camera(scene_camera c, const std::string& camera_tag)
        {
            set_scene_camera(c.eyex, c.eyey, c.eyez, c.centerx, c.centery, c.centerz, c.upx, c.upy, c.upz, camera_tag);
        }
        //-------------------------------------------------------------------------
        void set_scene_camera(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz, const std::string& camera_tag)
        {
            internal::_active_camera.set_position(eyex, eyey, eyez);
            internal::_active_camera.set_center(centerx, centery, centerz);
            internal::_active_camera.set_up_direction(upx, upy, upz);

            f32 width = environment::canvas_width();
            f32 height = environment::canvas_height();

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
        void activate_camera(const std::string& camera_tag)
        {
            camera_manager::camera* c = camera_manager::camera_by_tag(camera_tag);
            if (c)
            {
                camera_manager::set_as_active_camera(camera_tag);

                internal::_active_camera.eyex = c->eye.x; internal::_active_camera.eyey = c->eye.y; internal::_active_camera.eyez = c->eye.z;
                internal::_active_camera.centerx = c->target.x; internal::_active_camera.centery = c->target.y; internal::_active_camera.centerz = c->target.z;
                internal::_active_camera.upx = c->up.x; internal::_active_camera.upy = c->up.y; internal::_active_camera.upz = c->up.z;
            }
        }

        //-------------------------------------------------------------------------
        void ortho(float left, float right, float bottom, float top, float near, float far, const std::string& camera_tag)
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
        void perspective(float fovy, float aspect, float near, float far, const std::string& camera_tag)
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
        void orbit_control(orbit_scene_camera_options options, const std::string& camera_tag)
        {
            f32 zs = options.zoom_sensitivity != 0.0f ? options.zoom_sensitivity : internal::_zoom_sensitivity;
            f32 rs = options.rotation_sensitivity != 0.0f ? options.rotation_sensitivity : internal::_rotate_sensitivity;
            f32 ps = options.panning_sensitivity != 0.0f ? options.panning_sensitivity : internal::_pan_ensitivity;

            f32 delta_x = mouse::moved_x();
            f32 delta_y = mouse::moved_y();

            f32 scroll = mouse::scroll_offset_y();

            if (mouse::is_left_button_pressed())
            {
                internal::_active_camera.azimuth -= delta_x * rs * device::delta_time(); // Horizontal rotation (Azimuth)
                internal::_active_camera.polar -= delta_y * rs * device::delta_time();   // Vertical rotation (Polar)

                // Clamp polar angle to avoid flipping the camera
                internal::_active_camera.polar = glm::clamp(internal::_active_camera.polar, 0.1f, glm::pi<float>() - 0.1f);
            }

            if (mouse::is_right_button_pressed())
            {
                glm::vec3 cam_eye = { internal::_active_camera.eyex,internal::_active_camera.eyey,internal::_active_camera.eyez };
                glm::vec3 cam_target = { internal::_active_camera.centerx,internal::_active_camera.centery,internal::_active_camera.centerz };
                glm::vec3 cam_up = { internal::_active_camera.upx,internal::_active_camera.upy,internal::_active_camera.upz };

                glm::vec3 right = glm::normalize(glm::cross(cam_target - cam_eye, cam_up)); // Right vector
                glm::vec3 up = glm::normalize(up);                              // Up vector

                glm::vec3 pan_right = right * delta_x * ps * internal::_active_camera.radius * device::delta_time();
                glm::vec3 pan_up = up * delta_y * ps * internal::_active_camera.radius * device::delta_time();

                // Shift target position for panning
                cam_target += -pan_right + pan_up;

                internal::_active_camera.centerx = cam_target.x;
                internal::_active_camera.centery = cam_target.y;
                internal::_active_camera.centerz = cam_target.z;
            }

            if (scroll != 0.0f)
            {
                internal::_active_camera.radius -= scroll * zs * device::delta_time();

                // Clamp radius to avoid zooming too close or too far
                internal::_active_camera.radius = glm::clamp(internal::_active_camera.radius, options.min_zoom, options.max_zoom);
            }

            internal::_active_camera.eyex = internal::_active_camera.centerx + internal::_active_camera.radius * sin(internal::_active_camera.polar) * sin(internal::_active_camera.azimuth);
            internal::_active_camera.eyey = internal::_active_camera.centery + internal::_active_camera.radius * cos(internal::_active_camera.polar);  // Height
            internal::_active_camera.eyez = internal::_active_camera.centerz + internal::_active_camera.radius * sin(internal::_active_camera.polar) * cos(internal::_active_camera.azimuth);

            internal::push_active_camera(camera_tag);
        }
    }
}