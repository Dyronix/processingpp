add_executable(integration-test-model-create-obj)

target_sources(integration-test-model-create-obj PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-model-create-obj/private/crystal.h
${SOURCE_TEST_DIRECTORY}/integration-tests/test-model-create-obj/private/integration-test-model-create-obj.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-model-create-obj/private private)

target_include_directories(integration-test-model-create-obj PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(integration-test-model-create-obj processing_engine)

target_compile_definitions(integration-test-model-create-obj PRIVATE TEST_PROJECT_NAME=\"integration-test-model-create-obj\")

# Target properties
set_target_properties(integration-test-model-create-obj PROPERTIES
                      FOLDER "test/integration")