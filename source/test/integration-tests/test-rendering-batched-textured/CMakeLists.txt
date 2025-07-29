MESSAGE(STATUS "Adding integration-test-rendering-batched-textured")
add_executable(integration-test-rendering-batched-textured)

target_sources(integration-test-rendering-batched-textured PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-batched-textured/private/integration-test-rendering-batched-textured.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-batched-textured/private private)

target_include_directories(integration-test-rendering-batched-textured PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-rendering-batched-textured processing_engine)

target_compile_definitions(integration-test-rendering-batched-textured PRIVATE TEST_PROJECT_NAME=\"integration-test-rendering-batched-textured\")

# Target properties
set_target_properties(integration-test-rendering-batched-textured PROPERTIES
                      FOLDER "test/integration")