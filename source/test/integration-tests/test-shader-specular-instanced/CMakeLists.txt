add_executable(test-shader-specular-instanced)

target_sources(test-shader-specular-instanced PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-instanced/private/test-shader-specular-instanced.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-specular-instanced/private private)

target_include_directories(test-shader-specular-instanced PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-shader-specular-instanced processing_engine)

target_compile_definitions(test-shader-specular-instanced PRIVATE TEST_PROJECT_NAME=\"test-shader-specular-instanced\")

# Target properties
set_target_properties(test-shader-specular-instanced PROPERTIES
                      FOLDER "test/integration")