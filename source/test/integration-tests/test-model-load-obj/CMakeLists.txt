add_executable(integration-test-model-load-obj)

target_sources(integration-test-model-load-obj PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-model-load-obj/private/integration-test-model-load-obj.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-model-load-obj/private private)

target_include_directories(integration-test-model-load-obj PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-model-load-obj processing_engine)

target_compile_definitions(integration-test-model-load-obj PRIVATE TEST_PROJECT_NAME=\"integration-test-model-load-obj\")

# Target properties
set_target_properties(integration-test-model-load-obj PROPERTIES
                      FOLDER "test/integration")