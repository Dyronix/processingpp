add_executable(test-geometry-3d)

target_sources(test-geometry-3d PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-3d/private/test-geometry-3d.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-3d/private private)

target_include_directories(test-geometry-3d PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-geometry-3d processing_engine)

# Target properties
set_target_properties(test-geometry-3d PROPERTIES
                      FOLDER "test/integration")