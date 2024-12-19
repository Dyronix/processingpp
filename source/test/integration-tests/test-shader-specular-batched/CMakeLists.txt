add_executable(test-shader-specular-batched)

target_sources(test-shader-specular-batched PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-batched/private/test-shader-specular-batched.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-batched/private private)

target_include_directories(test-shader-specular-batched PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-shader-specular-batched processing_engine)

# Target properties
set_target_properties(test-shader-specular-batched PROPERTIES
                      FOLDER "test/integration")