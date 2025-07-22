add_executable(demo_imgui)

target_sources(demo_imgui PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_imgui/private/demo_imgui.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_imgui/private private)

target_include_directories(demo_imgui PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)
target_include_directories(demo_imgui PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public)

target_link_libraries(demo_imgui processing_framework)

# Target properties
set_target_properties(demo_imgui PROPERTIES
                      FOLDER "examples/framework")