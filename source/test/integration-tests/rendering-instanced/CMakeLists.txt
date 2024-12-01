add_executable(rendering-instanced)

target_sources(rendering-instanced PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/rendering-instanced/private/rendering-instanced.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/rendering-instanced/private private)

target_include_directories(rendering-instanced PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(rendering-instanced processing_engine)

# Target properties
set_target_properties(rendering-instanced PROPERTIES
                      FOLDER "test/integration")