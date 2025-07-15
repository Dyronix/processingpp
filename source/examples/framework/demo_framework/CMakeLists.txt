add_executable(demo_framework)

target_sources(demo_framework PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_framework/private/demo_framework.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/framework/demo_framework/private private)

target_include_directories(demo_framework PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)
target_include_directories(demo_framework PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public)

target_link_libraries(demo_framework processing_framework)

# Target properties
set_target_properties(demo_framework PROPERTIES
                      FOLDER "examples/framework")