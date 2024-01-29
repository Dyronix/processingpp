add_library(processing_engine STATIC)

# Sources
target_sources(processing_engine PRIVATE 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/shapes.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/environment.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/entrypoint.cpp)

target_sources(processing_engine PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/shapes.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/environment.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/entrypoint.h)

# Private includes
target_include_directories(processing_engine PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_utils/public)
target_include_directories(processing_engine PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/public)
target_include_directories(processing_engine PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_device/public)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)

target_link_libraries(processing_engine PUBLIC processing_utils) 
target_link_libraries(processing_engine PRIVATE processing_renderer) 
target_link_libraries(processing_engine PRIVATE processing_device) 
target_link_libraries(processing_engine PRIVATE glfw)

# Include self
target_include_directories(processing_engine PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

# Compile definitions
target_compile_definitions(processing_engine PUBLIC -DPPP_OPENGL)

if(${CMAKE_HOST_WIN32})
    target_compile_definitions(processing_engine PUBLIC -DPPP_WINDOWS)
endif()