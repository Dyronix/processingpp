#include "camera.h"
#include "environment.h"
#include "device/device_input.h"
#include "render/render.h"
#include "util/types.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ppp
{
    namespace camera
    {
        namespace internal
        {
            struct Ortho
            {
                float left, right, bottom, top = -1.0f;
                float near, far = -1.0f;
            };

            struct Perspective
            {
                float fovy, aspect = -1.0f;
                float near, far = -1.0f;
            };

            Ortho       _active_ortho = {};
            Perspective _active_perspective = {};           
            glm::mat4   _active_projection = glm::mat4(1.0f);
            bool        _active_projection_is_ortho = true;
            
            Camera      _active_camera = {};

            void push_active_camera()
            {
                render::push_active_camera(
                    {internal::_active_camera.eyex, internal::_active_camera.eyey, internal::_active_camera.eyez},              // eye
                    {internal::_active_camera.centerx, internal::_active_camera.centery, internal::_active_camera.centerz},     // center
                    {internal::_active_camera.upx, internal::_active_camera.upy, internal::_active_camera.upz},                 // up

                    internal::_active_projection);                                                                              // projection
            }
        }

        Camera::Camera()
        {
            eyex = 0.0f;
            eyey = 0.0f;
            eyez = 800.0f;

            centerx = 0.0f;
            centery = 0.0f;
            centerz = 0.0f;

            upx = 0.0f;
            upy = 1.0f;
            upz = 0.0f;
        }

        void Camera::set_position(float in_eyex, float in_eyey, float in_eyez)
        {
            eyex = in_eyex;
            eyey = in_eyey; 
            eyez = in_eyez;
        }
        
        void Camera::set_up_direction(float in_upx, float in_upy, float in_upz)
        {
            upx = in_upx;
            upy = in_upy;
            upz = in_upz;
        }
        
        void Camera::set_center(float in_centerx, float in_centery, float in_centerz)
        {
            centerx = in_centerx;
            centery = in_centery;
            centerz = in_centerz;
        }

        Camera create_camera()
        {
            Camera c = Camera();

            set_camera(c);

            return internal::_active_camera;
        }
        
        void set_camera(Camera c)
        {
            camera(c.eyex, c.eyey, c.eyez, c.centerx, c.centery, c.centerz, c.upx, c.upy, c.upz);
        }

        void camera(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz)
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

                ortho(left, right, bottom, top, near, far); // will push the active camera
            }
            else
            {
                f32 fovy = internal::_active_perspective.fovy == -1.0f ? 2 * atan((height * 0.5f) / 800.0f) : internal::_active_perspective.fovy;
                f32 aspect = internal::_active_perspective.aspect == -1.0f ? width / height : internal::_active_perspective.aspect;
                f32 near = internal::_active_perspective.near == -1.0f ? 0.1f * 800.0f : internal::_active_perspective.near;
                f32 far = internal::_active_perspective.far == -1.0f ? 10.0f * 800.0f : internal::_active_perspective.far;

                perspective(fovy, aspect, near, far); // will push the active camera
            }
        }

        void ortho(float left, float right, float bottom, float top, float near, float far)
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

            internal::push_active_camera();
        }

        void perspective(float fovy, float aspect, float near, float far)
        {
            internal::_active_perspective.fovy = fovy;
            internal::_active_perspective.aspect = aspect;
            internal::_active_perspective.near = near;
            internal::_active_perspective.far = far;

            internal::_active_projection_is_ortho = false;
            internal::_active_projection = glm::perspective(
                fovy,   
                aspect, 
                near,   
                far);   

            internal::push_active_camera();
        }

        void orbit_control()
        {
            
        }
    }
}