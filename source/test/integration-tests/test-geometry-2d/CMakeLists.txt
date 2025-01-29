add_executable(test-geometry-2d)

target_sources(test-geometry-2d PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-2d/private/test-geometry-2d.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-geometry-2d/private private)

target_include_directories(test-geometry-2d PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-geometry-2d processing_engine)

target_compile_definitions(test-geometry-2d PRIVATE TEST_PROJECT_NAME=\"test-geometry-2d\")

# Target properties
set_target_properties(test-geometry-2d PROPERTIES
                      FOLDER "test/integration")