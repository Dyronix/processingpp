add_executable(test-geometry-gpu)

target_sources(test-geometry-gpu PRIVATE 
    ${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-gpu/private/test-geometry-gpu.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-gpu/private private)

target_include_directories(test-geometry-gpu PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-geometry-gpu processing_engine)

# Target properties
set_target_properties(test-geometry-gpu PROPERTIES
                      FOLDER "test/integration")