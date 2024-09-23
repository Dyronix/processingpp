add_executable(wave-function-collapse)

target_sources(wave-function-collapse PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/wave-function-collapse/private/tile.h
    ${SOURCE_EXAMPLES_DIRECTORY}/wave-function-collapse/private/tile.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/wave-function-collapse/private/cell.h
    ${SOURCE_EXAMPLES_DIRECTORY}/wave-function-collapse/private/cell.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/wave-function-collapse/private/wave-function-collapse.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/wave-function-collapse/private private)

target_include_directories(wave-function-collapse PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(wave-function-collapse processing_engine)

# Target properties
set_target_properties(wave-function-collapse PROPERTIES
                      FOLDER "examples")