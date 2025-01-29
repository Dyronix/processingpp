add_executable(test-shader-normal-batched)

target_sources(test-shader-normal-batched PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-batched/private/test-shader-normal-batched.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-batched/private private)

target_include_directories(test-shader-normal-batched PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-shader-normal-batched processing_engine)

target_compile_definitions(test-shader-normal-batched PRIVATE TEST_PROJECT_NAME=\"test-shader-normal-batched\")

# Target properties
set_target_properties(test-shader-normal-batched PROPERTIES
                      FOLDER "test/integration")