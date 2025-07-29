MESSAGE(STATUS "Adding integration-test-shader-normal-instanced")
add_executable(integration-test-shader-normal-instanced)

target_sources(integration-test-shader-normal-instanced PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-instanced/private/integration-test-shader-normal-instanced.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-instanced/private private)

target_include_directories(integration-test-shader-normal-instanced PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-shader-normal-instanced processing_engine)

target_compile_definitions(integration-test-shader-normal-instanced PRIVATE TEST_PROJECT_NAME=\"integration-test-shader-normal-instanced\")

# Target properties
set_target_properties(integration-test-shader-normal-instanced PROPERTIES
                      FOLDER "test/integration")