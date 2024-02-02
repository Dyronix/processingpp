add_library(processing_engine STATIC)

# Sources
target_sources(processing_engine PRIVATE 
    # fileio
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/fileio.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/fileio.cpp
    # device
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/device.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/device_gl.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/device_input.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/device_input.cpp
    # render
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_gl.cpp
    # util
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/hash.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/log.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/string_ops.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/string_ops.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/types.h
    # engine
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/shapes.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/transform.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/color.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/environment.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/structure.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/constants.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/rendering.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/events.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/data.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/mathematics.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/trigonometry.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/image.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/entrypoint.cpp)

target_sources(processing_engine PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/shapes.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/transform.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/color.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/environment.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/structure.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/constants.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/rendering.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/events.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/data.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/mathematics.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/trigonometry.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/image.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/entrypoint.h)

# Create the project filters
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private private)
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public public)

# Private includes
target_include_directories(processing_engine PUBLIC ${SOURCE_THIRDPARTY_DIRECTORY}/glm)

target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/stb/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/fmt/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/stb/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glad/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)

target_link_libraries(processing_engine PRIVATE stb)
target_link_libraries(processing_engine PRIVATE glad)
target_link_libraries(processing_engine PRIVATE glfw)

# Include self
target_include_directories(processing_engine PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private)
target_include_directories(processing_engine PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

# Compile definitions
target_compile_definitions(processing_engine PUBLIC -DPPP_OPENGL)

if(${CMAKE_HOST_WIN32})
    target_compile_definitions(processing_engine PUBLIC -DPPP_WINDOWS)
endif()

# Target properties
set_target_properties(processing_engine PROPERTIES
                      FOLDER "engine")