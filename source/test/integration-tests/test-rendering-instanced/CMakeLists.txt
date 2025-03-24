add_executable(integration-test-rendering-instanced)

target_sources(integration-test-rendering-instanced PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced/private/integration-test-rendering-instanced.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced/private private)

target_include_directories(integration-test-rendering-instanced PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-rendering-instanced processing_engine)

target_compile_definitions(integration-test-rendering-instanced PRIVATE TEST_PROJECT_NAME=\"test-rendering-instanced\")

# Target properties
set_target_properties(integration-test-rendering-instanced PROPERTIES
                      FOLDER "test/integration")