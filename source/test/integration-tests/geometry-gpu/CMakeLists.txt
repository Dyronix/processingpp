add_executable(geometry-gpu)

target_sources(geometry-gpu PRIVATE 
    ${SOURCE_TEST_DIRECTORY}/integration-tests/geometry-gpu/private/geometry-gpu.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/geometry-gpu/private private)

target_include_directories(geometry-gpu PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(geometry-gpu processing_engine)

# Target properties
set_target_properties(geometry-gpu PROPERTIES
                      FOLDER "test/integration")