add_executable(rendering-batched)

target_sources(rendering-batched PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/rendering-batched/private/rendering-batched.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/rendering-batched/private private)

target_include_directories(rendering-batched PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(rendering-batched processing_engine)

# Target properties
set_target_properties(rendering-batched PROPERTIES
                      FOLDER "test/integration")