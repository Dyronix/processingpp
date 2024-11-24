add_executable(geometry-3d)

target_sources(geometry-3d PRIVATE 
${SOURCE_TEST_DIRECTORY}/integration-tests/geometry-3d/private/geometry-3d.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_TEST_DIRECTORY}/integration-tests/geometry-3d/private private)

target_include_directories(geometry-3d PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(geometry-3d processing_engine)

# Target properties
set_target_properties(geometry-3d PROPERTIES
                      FOLDER "test/integration")