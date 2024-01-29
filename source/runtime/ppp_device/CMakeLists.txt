add_library(processing_device STATIC)

# Sources
target_sources(processing_device PRIVATE
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_device/private/device_input.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_device/private/device_input.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_device/private/device_gl.cpp)

target_sources(processing_device PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_device/public/device.h)

# Private includes
target_include_directories(processing_device PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)

target_link_libraries(processing_device PRIVATE glfw)

# Public includes
target_include_directories(processing_device PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_utils/public)

target_link_libraries(processing_device PUBLIC processing_utils)

# Include self
target_include_directories(processing_device PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_device/public)

# Compile definitions
target_compile_definitions(processing_device PUBLIC -DPPP_OPENGL)

if(${CMAKE_HOST_WIN32})
    target_compile_definitions(processing_device PUBLIC -DPPP_WINDOWS)
endif()