add_executable(test-rendering-batched-textured)

target_sources(test-rendering-batched-textured PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-batched-textured/private/test-rendering-batched-textured.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-rendering-batched-textured/private private)

target_include_directories(test-rendering-batched-textured PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-rendering-batched-textured processing_engine)

# Target properties
set_target_properties(test-rendering-batched-textured PROPERTIES
                      FOLDER "test/integration")