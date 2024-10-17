namespace ppp
{
    namespace camera
    {
        struct Camera
        {
            Camera();

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

        struct OrbitCameraOptions
        {
            float zoom_sensitivity = 0.0f;
            float rotation_sensitivity = 0.0f;
            float panning_sensitivity = 0.0f;

            float min_zoom = 1.0f;
            float max_zoom = 100.0f;
        };

        Camera create_camera();

        void set_camera(Camera c);
        void camera(float eyex, float eyey, float eyez = 800.0f, float centerx = 0.0f, float centery = 0.0f, float centerz = 0.0f, float upx = 0.0f, float upy = 1.0f, float upz = 0.0f);

        void ortho(float left, float right, float bottom, float top, float near, float far);
        void perspective(float fovy, float aspect, float near, float far);

        void orbit_control(OrbitCameraOptions options = {});
    }
}