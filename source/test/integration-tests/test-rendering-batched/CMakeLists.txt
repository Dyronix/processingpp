add_executable(integration-test-rendering-batched)

target_sources(integration-test-rendering-batched PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-batched/private/integration-test-rendering-batched.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-batched/private private)

target_include_directories(integration-test-rendering-batched PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-rendering-batched processing_engine)

target_compile_definitions(integration-test-rendering-batched PRIVATE TEST_PROJECT_NAME=\"test-rendering-batched\")

# Target properties
set_target_properties(integration-test-rendering-batched PROPERTIES
                      FOLDER "test/integration")