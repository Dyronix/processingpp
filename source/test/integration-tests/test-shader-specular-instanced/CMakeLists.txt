add_executable(integration-test-shader-specular-instanced)

target_sources(integration-test-shader-specular-instanced PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-instanced/private/integration-test-shader-specular-instanced.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-instanced/private private)

target_include_directories(integration-test-shader-specular-instanced PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-shader-specular-instanced processing_engine)

target_compile_definitions(integration-test-shader-specular-instanced PRIVATE TEST_PROJECT_NAME=\"integration-test-shader-specular-instanced\")

# Target properties
set_target_properties(integration-test-shader-specular-instanced PROPERTIES
                      FOLDER "test/integration")