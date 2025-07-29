MESSAGE(STATUS "Adding example-demo_framework_imgui")
add_executable(demo_framework_imgui)

target_sources(demo_framework_imgui PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_framework_imgui/private/demo_framework_imgui.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_framework_imgui/private private)

target_include_directories(demo_framework_imgui PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)
target_include_directories(demo_framework_imgui PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public)

target_link_libraries(demo_framework_imgui processing_framework)

# Target properties
set_target_properties(demo_framework_imgui PROPERTIES
                      FOLDER "examples/framework")