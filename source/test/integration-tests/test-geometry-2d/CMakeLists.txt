MESSAGE(STATUS "Adding integration-test-geometry-2d")
add_executable(integration-test-geometry-2d)

target_sources(integration-test-geometry-2d PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-2d/private/integration-test-geometry-2d.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-2d/private private)

target_include_directories(integration-test-geometry-2d PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-geometry-2d processing_engine)

target_compile_definitions(integration-test-geometry-2d PRIVATE TEST_PROJECT_NAME=\"integration-test-geometry-2d\")

# Target properties
set_target_properties(integration-test-geometry-2d PROPERTIES
                      FOLDER "test/integration")