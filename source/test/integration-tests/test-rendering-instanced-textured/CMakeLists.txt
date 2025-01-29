add_executable(test-rendering-instanced-textured)

target_sources(test-rendering-instanced-textured PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced-textured/private/test-rendering-instanced-textured.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced-textured/private private)

target_include_directories(test-rendering-instanced-textured PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-rendering-instanced-textured processing_engine)

target_compile_definitions(test-rendering-instanced-textured PRIVATE TEST_PROJECT_NAME=\"test-rendering-instanced-textured\")

# Target properties
set_target_properties(test-rendering-instanced-textured PROPERTIES
                      FOLDER "test/integration")