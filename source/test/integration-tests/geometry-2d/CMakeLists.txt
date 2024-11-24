add_executable(geometry-2d)

target_sources(geometry-2d PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/geometry-2d/private/geometry-2d.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/geometry-2d/private private)

target_include_directories(geometry-2d PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(geometry-2d processing_engine)

# Target properties
set_target_properties(geometry-2d PROPERTIES
                      FOLDER "test/integration")