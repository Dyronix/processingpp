add_executable(test-shader-normal-instanced)

target_sources(test-shader-normal-instanced PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-instanced/private/test-shader-normal-instanced.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/test-shader-normal-instanced/private private)

target_include_directories(test-shader-normal-instanced PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(test-shader-normal-instanced processing_engine)

# Target properties
set_target_properties(test-shader-normal-instanced PROPERTIES
                      FOLDER "test/integration")