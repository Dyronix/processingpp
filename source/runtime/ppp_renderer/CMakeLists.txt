add_library(processing_renderer STATIC)

target_sources(processing_renderer PRIVATE 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/private/render_gl.cpp)

target_sources(processing_renderer PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/public/render.h)

target_include_directories(processing_renderer PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glad/include)

target_include_directories(processing_renderer PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/public)
target_link_libraries(processing_renderer glad)

target_compile_definitions(processing_renderer PUBLIC -DPPP_OPENGL)

if(${CMAKE_HOST_WIN32})
    target_compile_definitions(processing_renderer PUBLIC -DPPP_WINDOWS)
endif()