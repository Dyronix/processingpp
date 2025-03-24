add_executable(integration-test-geometry-3d)

target_sources(integration-test-geometry-3d PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-3d/private/integration-test-geometry-3d.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-3d/private private)

target_include_directories(integration-test-geometry-3d PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-geometry-3d processing_engine)

target_compile_definitions(integration-test-geometry-3d PRIVATE TEST_PROJECT_NAME=\"test-geometry-3d\")

# Target properties
set_target_properties(integration-test-geometry-3d PROPERTIES
                      FOLDER "test/integration")