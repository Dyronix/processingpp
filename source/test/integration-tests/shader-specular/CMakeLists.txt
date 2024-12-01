add_executable(shader-specular)

target_sources(shader-specular PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/shader-specular/private/shader-specular.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/shader-specular/private private)

target_include_directories(shader-specular PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(shader-specular processing_engine)

# Target properties
set_target_properties(shader-specular PROPERTIES
                      FOLDER "test/integration")