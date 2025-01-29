add_executable(test-rendering-instanced)

target_sources(test-rendering-instanced PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced/private/test-rendering-instanced.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-instanced/private private)

target_include_directories(test-rendering-instanced PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-rendering-instanced processing_engine)

target_compile_definitions(test-rendering-instanced PRIVATE TEST_PROJECT_NAME=\"test-rendering-instanced\")

# Target properties
set_target_properties(test-rendering-instanced PROPERTIES
                      FOLDER "test/integration")