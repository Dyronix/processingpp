add_executable(demo_engine)

target_sources(demo_engine PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/demo_engine/private/demo_engine.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/engine/demo_engine/private private)

target_include_directories(demo_engine PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(demo_engine processing_engine)

# Target properties
set_target_properties(demo_engine PROPERTIES
                      FOLDER "examples/engine")