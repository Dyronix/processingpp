add_executable(shader-normal)

target_sources(shader-normal PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/shader-normal/private/shader-normal.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/shader-normal/private private)

target_include_directories(shader-normal PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(shader-normal processing_engine)

# Target properties
set_target_properties(shader-normal PROPERTIES
                      FOLDER "test/integration")