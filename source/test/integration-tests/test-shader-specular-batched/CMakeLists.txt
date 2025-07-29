MESSAGE(STATUS "Adding integration-test-shader-specular-batched")
add_executable(integration-test-shader-specular-batched)

target_sources(integration-test-shader-specular-batched PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-batched/private/integration-test-shader-specular-batched.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-batched/private private)

target_include_directories(integration-test-shader-specular-batched PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-shader-specular-batched processing_engine)

target_compile_definitions(integration-test-shader-specular-batched PRIVATE TEST_PROJECT_NAME=\"integration-test-shader-specular-batched\")

# Target properties
set_target_properties(integration-test-shader-specular-batched PROPERTIES
                      FOLDER "test/integration")