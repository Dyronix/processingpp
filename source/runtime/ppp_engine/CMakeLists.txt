add_library(processing_engine STATIC)

target_sources(processing_engine PRIVATE 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/shapes.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/entrypoint.cpp)

target_sources(processing_engine PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/types.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/shapes.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/entrypoint.h)

target_include_directories(processing_engine PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_renderer/public)
target_link_libraries(processing_engine PRIVATE processing_renderer) 

target_include_directories(processing_engine PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)
target_include_directories(processing_engine PUBLIC ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)
target_include_directories(processing_engine PUBLIC ${SOURCE_THIRDPARTY_DIRECTORY}/glad/include)
target_link_libraries(processing_engine PUBLIC glfw)

target_compile_definitions(processing_engine PUBLIC -DPPP_OPENGL)

if(${CMAKE_HOST_WIN32})
    target_compile_definitions(processing_engine PUBLIC -DPPP_WINDOWS)
endif()