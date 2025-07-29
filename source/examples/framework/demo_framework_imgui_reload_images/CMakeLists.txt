MESSAGE(STATUS "Adding example-demo_framework_imgui_reload_images")
add_executable(demo_framework_imgui_reload_images)

target_sources(demo_framework_imgui_reload_images PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_framework_imgui_reload_images/private/demo_framework_imgui_reload_images.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_framework_imgui_reload_images/private private)

target_include_directories(demo_framework_imgui_reload_images PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)
target_include_directories(demo_framework_imgui_reload_images PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public)

target_link_libraries(demo_framework_imgui_reload_images processing_framework)

# Target properties
set_target_properties(demo_framework_imgui_reload_images PROPERTIES
                      FOLDER "examples/framework")