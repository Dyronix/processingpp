#include "entrypoint.h"

#include <iostream>

#if PPP_OPENGL
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

namespace ppp
{
    // forward declaration
#if PPP_OPENGL
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
#endif
    void process_input(GLFWwindow *window);

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void process_input(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

#if PPP_OPENGL
    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
#endif

    //-------------------------------------------------------------------------
    int rex_entry()
    {
        AppParams app_params = ppp::app_entry();

        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
    #if PPP_OPENGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef PPP_APPLE
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    #else
        glfwWindowHint(GLFW_NO_API, GLFW_CLIENT_API);
    #endif

        // glfw window creation
        // --------------------
        GLFWwindow *window = glfwCreateWindow(app_params.window_width, app_params.window_height, "Processing", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }

    #if PPP_OPENGL
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }
    #endif

        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----
            process_input(window);

    #if PPP_OPENGL
            // render
            // ------
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
    #endif
        }

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        glfwTerminate();
        return 0;
    }
}

int main()
{
    return ppp::rex_entry();
}
