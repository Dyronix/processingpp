add_executable(integration-test-rendering-instanced-textured)

target_sources(integration-test-rendering-instanced-textured PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced-textured/private/integration-test-rendering-instanced-textured.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced-textured/private private)

target_include_directories(integration-test-rendering-instanced-textured PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-rendering-instanced-textured processing_engine)

target_compile_definitions(integration-test-rendering-instanced-textured PRIVATE TEST_PROJECT_NAME=\"integration-test-rendering-instanced-textured\")

# Target properties
set_target_properties(integration-test-rendering-instanced-textured PROPERTIES
                      FOLDER "test/integration")