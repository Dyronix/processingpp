add_library(processing_renderer STATIC)

# Sources
target_sources(processing_renderer PRIVATE 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/private/render_gl.cpp)

target_sources(processing_renderer PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/public/render.h)

# Private includes
target_include_directories(processing_renderer PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glad/include)

target_link_libraries(processing_renderer PRIVATE glad)

# Public includes
target_include_directories(processing_renderer PUBLIC ${SOURCE_THIRDPARTY_DIRECTORY}/glm)
target_include_directories(processing_renderer PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_utils/public)

target_link_libraries(processing_renderer PUBLIC processing_utils)

# Include self
target_include_directories(processing_renderer PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/public)

# Compile definitions
target_compile_definitions(processing_renderer PUBLIC -DPPP_OPENGL)

if(${CMAKE_HOST_WIN32})
    target_compile_definitions(processing_renderer PUBLIC -DPPP_WINDOWS)
endif()