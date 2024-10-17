#include "entrypoint.h"
#include "environment.h"
#include "color.h"

#include "device/device.h"
#include "render/render.h"
#include "resources/texture_pool.h"
#include "resources/font_pool.h"
#include "resources/shader_pool.h"
#include "fileio/fileio.h"

#include "util/log.h"
#include "util/types.h"
#include "util/steady_clock.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ppp
{
    namespace internal
    {
        //-------------------------------------------------------------------------
        std::string get_working_directory(const std::string& executable_path)
        {
            size_t last_slash_pos = executable_path.find_last_of("/\\");

            return executable_path.substr(0, last_slash_pos + 1);
        }
    }

    GLuint VAO, VBO;
    std::vector<glm::vec3> points; // This should be filled with your grid of points
    GLuint shaderProgram;

    std::vector<glm::vec3> generateGridPoints(int rows, int cols, float spacing) {
        //std::vector<glm::vec3> points;
        //for (int z = 0; z < rows; ++z) {
        //    for (int x = 0; x < cols; ++x) {
        //        float xPos = (x - cols / 2) * spacing;  // Center the grid at (0, 0, 0)
        //        float zPos = (z - rows / 2) * spacing;  // Center the grid at (0, 0, 0)
        //        points.push_back(glm::vec3(xPos, 0.0f, zPos));
        //    }
        //}
        //return points;
        return { glm::vec3(0.0f, 0.0f, 0.0f) };
    }

    // Function to load shader source code from file
    std::string loadShaderSource(const char* filepath) {
        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // Function to compile shaders
    GLuint compileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        // Check for compilation errors
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        return shader;
    }

    // Function to link shaders into a program
    GLuint createShaderProgram(const char* vertexSource, const char* geometrySource, const char* fragmentSource) {
        // Compile the vertex shader
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);

        // Compile the geometry shader
        GLuint geometryShader = compileShader(GL_GEOMETRY_SHADER, geometrySource);

        // Compile the fragment shader
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        // Create shader program and link the shaders
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, geometryShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // Check for linking errors
        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Clean up the shaders since we no longer need them
        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    //-------------------------------------------------------------------------
    s32 init(const AppParams& app_params, const std::string& executable_path)
    {
        if (!device::initialize(app_params.window_width, app_params.window_height))
        {
            return -1;
        }

        #if PPP_OPENGL
        if (!render::initialize(app_params.window_width, app_params.window_height, glfwGetProcAddress))
        {
            return -1;
        }
        #endif

        if (!texture_pool::initialize())
        {
            return -1;
        }

        if(!font_pool::initialize())
        {
            return -1;
        }

        if(!shader_pool::initialize())
        {
            return -1;
        }

        fileio::add_wildcard("local:", internal::get_working_directory(executable_path));

        color::background(1.0f, 1.0f, 1.0f, 1.0f);

        setup();

        // Define the grid size and spacing
        int gridRows = 50;        // Number of rows (Z direction)
        int gridCols = 50;        // Number of columns (X direction)
        float gridSpacing = 10.0f; // Distance between points

        // Generate the grid points
        points = generateGridPoints(gridRows, gridCols, gridSpacing);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        // Load shader sources (or write them directly as strings)
        const char* vertexSource = R"(
        #version 330 core
        layout(location = 0) in vec3 inPosition;
        uniform mat4 v;
        uniform mat4 p;

        void main() {
            gl_Position = vec4(inPosition, 1.0);
        }
        )";

        const char* geometrySource = R"(
        #version 330 core

        layout(points) in;
        layout(triangle_strip, max_vertices = 24) out;

        uniform mat4 vp;

uniform float amplitudeX;
uniform float amplitudeZ;
uniform float movementSpeedX;
uniform float movementSpeedZ;
uniform float periodicScaleX;
uniform float periodicScaleZ;
uniform float time;

        out vec3 fragColor;

void GenerateCube(vec3 center)
{
    float cubeSize = 20; // Size of the cube

    // Cube vertices relative to the center
    vec3 cubeVertices[8] = vec3[8](
        vec3(-cubeSize, -cubeSize, -cubeSize), // 0
        vec3(cubeSize, -cubeSize, -cubeSize),  // 1
        vec3(cubeSize,  cubeSize, -cubeSize),  // 2
        vec3(-cubeSize,  cubeSize, -cubeSize), // 3
        vec3(-cubeSize, -cubeSize,  cubeSize), // 4
        vec3(cubeSize, -cubeSize,  cubeSize),  // 5
        vec3(cubeSize,  cubeSize,  cubeSize),  // 6
        vec3(-cubeSize,  cubeSize,  cubeSize)  // 7
    );

    int cubeIndices[24] = int[24](
        0, 1, 3, 2, // Back face
        1, 5, 2, 6, // Right face
        5, 4, 6, 7, // Front face
        4, 0, 7, 3, // Left face
        3, 2, 7, 6, // Top face
        4, 5, 0, 1  // Bottom face
    );

    for (int i = 0; i < 24; i++)
    {
        vec3 pos = center + cubeVertices[cubeIndices[i]];
        gl_Position = vp * vec4(pos, 1.0);
        EmitVertex();
        if (i % 4 == 3)
        {
            EndPrimitive(); // End primitive after every face
        }
    }
}

        void main() {
    //        vec3 position = gl_in[0].gl_Position.xyz;

    //// Compute the wave heights
    //float waveHeightX = sin((position.x * periodicScaleX) + movementSpeedX * time) * amplitudeX;
    //float waveHeightZ = sin((position.z * periodicScaleZ) + movementSpeedZ * time) * amplitudeZ;
    //float y = waveHeightX + waveHeightZ;

    //// Interpolating color based on height
    //float minHeight = -amplitudeX - amplitudeZ;
    //float maxHeight = amplitudeX + amplitudeZ;
    //float colorFactor = (y - minHeight) / (maxHeight - minHeight);
    //fragColor = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), colorFactor);
            
            //int cubeFaceIndices[36] = int[36](
            //        // Front face
            //        0, 1, 2, 0, 2, 3,
            //        // Back face
            //        4, 6, 5, 4, 7, 6,
            //        // Left face
            //        8, 9, 10, 8, 10, 11,
            //        // Right face
            //        12, 14, 13, 12, 15, 14,
            //        // Bottom face
            //        16, 18, 17, 16, 19, 18,
            //        // Top face
            //        20, 21, 22, 20, 22, 23
            //    );

            //// Define cube vertices (in model space)
            //float width = 20;
            //float height = 20;
            //float depth = 20;

            //    int index = 0;

            //    vec3 cubeVertices[24];

            //    // Front face
            //    cubeVertices[index++] = vec3(-width / 2, -height / 2, depth / 2); // Bottom Left
            //    cubeVertices[index++] = vec3(width / 2, -height / 2, depth / 2);  // Bottom Right
            //    cubeVertices[index++] = vec3(width / 2, height / 2, depth / 2);   // Top Right
            //    cubeVertices[index++] = vec3(-width / 2, height / 2, depth / 2);  // Top Left

            //    // Back face
            //    cubeVertices[index++] = vec3(-width / 2, -height / 2, -depth / 2); // Bottom Left
            //    cubeVertices[index++] = vec3(width / 2, -height / 2, -depth / 2);  // Bottom Right
            //    cubeVertices[index++] = vec3(width / 2, height / 2, -depth / 2);   // Top Right
            //    cubeVertices[index++] = vec3(-width / 2, height / 2, -depth / 2);  // Top Left

            //    // Left face
            //    cubeVertices[index++] = vec3(-width / 2, -height / 2, -depth / 2); // Bottom Back
            //    cubeVertices[index++] = vec3(-width / 2, -height / 2, depth / 2);  // Bottom Front
            //    cubeVertices[index++] = vec3(-width / 2, height / 2, depth / 2);   // Top Front
            //    cubeVertices[index++] = vec3(-width / 2, height / 2, -depth / 2);  // Top Back

            //    // Right face
            //    cubeVertices[index++] = vec3(width / 2, -height / 2, -depth / 2);  // Bottom Back
            //    cubeVertices[index++] = vec3(width / 2, -height / 2, depth / 2);   // Bottom Front
            //    cubeVertices[index++] = vec3(width / 2, height / 2, depth / 2);    // Top Front
            //    cubeVertices[index++] = vec3(width / 2, height / 2, -depth / 2);   // Top Back

            //    // Top face
            //    cubeVertices[index++] = vec3(-width / 2, height / 2, -depth / 2);  // Back Left
            //    cubeVertices[index++] = vec3(width / 2, height / 2, -depth / 2);   // Back Right
            //    cubeVertices[index++] = vec3(width / 2, height / 2, depth / 2);    // Front Right
            //    cubeVertices[index++] = vec3(-width / 2, height / 2, depth / 2);   // Front Left

            //    // Bottom face
            //    cubeVertices[index++] = vec3(-width / 2, -height / 2, -depth / 2); // Back Left
            //    cubeVertices[index++] = vec3(width / 2, -height / 2, -depth / 2);  // Back Right
            //    cubeVertices[index++] = vec3(width / 2, -height / 2, depth / 2);   // Front Right
            //    cubeVertices[index++] = vec3(-width / 2, -height / 2, depth / 2);  // Front Left

            //for(int i = 0; i < 36; ++i)
            //{
            //        int idx = cubeFaceIndices[i];
            //        gl_Position = vp * vec4(position + cubeVertices[idx] + vec3(0.0, y, 0.0), 1.0);
            //        EmitVertex();
            //}

            //EndPrimitive();

    vec3 pointPosition = gl_in[0].gl_Position.xyz; // The input point
    GenerateCube(pointPosition);
        }
        )";

        const char* fragmentSource = R"(
        #version 330 core
        in vec3 fragColor;
        out vec4 outColor;

        void main() {
            outColor = vec4(fragColor, 1.0);
        }
        )";

        // Create the shader program
        shaderProgram = createShaderProgram(vertexSource, geometrySource, fragmentSource);


        return 0;
    }
    //-------------------------------------------------------------------------
    s32 run(const AppParams& app_params)
    {
        while (!device::should_close())
        {
            if (device::can_draw())
            {
                // render
                // ------
                render::begin();

                draw();

                render::render(shaderProgram, device::current_frame_index(), VAO, points);

                render::end();

                // swap front/back buffers
                // -----
                device::present();
            }

            // poll new window events
            // ----
            device::tick();
            device::poll_events();

            // force app to run at a certain framerate
            // ----
            if (device::is_looping())
            {
                if (device::delta_time() < device::max_frame_time())
                {
                    f32 sleep_time_seconds = device::max_frame_time() - device::delta_time();

                    // Convert seconds to milliseconds for sleeping
                    clock::milliseconds sleep_time(static_cast<s32>(sleep_time_seconds * 1000));
                    clock::accurate_sleep_for(sleep_time);
                }
            }
        }

        return 0;
    }
    //-------------------------------------------------------------------------
    s32 quit(const AppParams& app_params)
    {
        // Clean up resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        font_pool::terminate();
        texture_pool::terminate();
        render::terminate();
        device::terminate();

        return 0;
    }
}

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; ++i)
    {
        ppp::log::info("Application argument: {}", argv[i]);
    }

    ppp::AppParams app_params = ppp::entry();

    s32 result = 0;

    result = ppp::init(app_params, argv[0]);
    if (result != 0)
    {
        ppp::log::error("Failed to initialize app");
        return result;
    }
    result = run(app_params);
    if (result != 0)
    {
        ppp::log::error("Encountered runtime error in app");
        return result;
    }
    result = quit(app_params);
    if (result != 0)
    {
        ppp::log::error("Failed to quit app");
        return result;
    }

    return result;
}
