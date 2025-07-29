MESSAGE(STATUS "Adding integration-test-shader-normal-batched")
add_executable(integration-test-shader-normal-batched)

target_sources(integration-test-shader-normal-batched PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-batched/private/integration-test-shader-normal-batched.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-batched/private private)

target_include_directories(integration-test-shader-normal-batched PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-shader-normal-batched processing_engine)

target_compile_definitions(integration-test-shader-normal-batched PRIVATE TEST_PROJECT_NAME=\"integration-test-shader-normal-batched\")

# Target properties
set_target_properties(integration-test-shader-normal-batched PROPERTIES
                      FOLDER "test/integration")