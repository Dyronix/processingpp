#include "device_input.h"

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace device
    {
        namespace input
        {
            namespace internal
            {
                // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
                // ---------------------------------------------------------------------------------------------------------
                void process_input(GLFWwindow* window)
                {
                    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                    {
                        glfwSetWindowShouldClose(window, true);
                    }
                }
            }

            void process_input(GLFWwindow* window)
            {
                internal::process_input(window);
            }
        }
    }
}