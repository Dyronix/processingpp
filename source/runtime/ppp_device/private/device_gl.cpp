#include "device.h"
#include "device_input.h"

#include "log.h"

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace device
    {
        namespace internal
        {
            GLFWwindow* _window;
        }

        bool initialize(s32 w, s32 h)
        {
            // glfw: initialize and configure
            // ------------------------------
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            #ifdef PPP_APPLE
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            #endif

            // glfw window creation
            // --------------------
            internal::_window = glfwCreateWindow(w, h, "Processing", NULL, NULL);
            if (internal::_window == NULL)
            {
                log::error("Failed to create GLFW window");

                terminate();
                return false;
            }

            glfwMakeContextCurrent(internal::_window);
            glfwSwapInterval(1);

            return true;
        }
        void terminate()
        {
            // glfw: terminate, clearing all previously allocated GLFW resources.
            // ------------------------------------------------------------------
            glfwTerminate();
        }

        GLFWwindow* window_handle()
        {
            return internal::_window;
        }

        void window_width(s32* w)
        {
            s32 height = 0;
            glfwGetWindowSize(internal::_window, w, &height);
        }
        void window_height(s32* h)
        {
            s32 width = 0;
            glfwGetWindowSize(internal::_window, &width, h);
        }

        void set_framebuffer_resize_callback(std::function<void(s32, s32, s32, s32)> callback)
        {
            // Cannot use closures in our lambda expression.
            // So we must capture this, and load it again when we require access to it.
            auto lambda = [](GLFWwindow* window, s32 width, s32 height)
            {
                std::function<void(s32, s32, s32, s32)>* callback = (std::function<void(s32, s32, s32, s32)>*)glfwGetWindowUserPointer(window);
                (*callback)(0, 0, width, height);
            };

            glfwSetWindowUserPointer(internal::_window, &callback);
            glfwSetFramebufferSizeCallback(internal::_window, lambda);
        }

        void poll_events()
        {
            glfwPollEvents();
        }

        void process_input()
        {
            input::process_input(internal::_window);
        }

        void present()
        {
            glfwSwapBuffers(internal::_window);
        }

        bool should_close()
        {
            return glfwWindowShouldClose(internal::_window);
        }
    }
}