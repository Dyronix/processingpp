add_executable(treasure-hunt)

target_sources(treasure-hunt PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private/file_reader.h
    ${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private/nonogram_grid.h
    ${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private/nonogram_parser.h
    ${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private/nonogram_printer.h
    ${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private/nonogram_puzzle.h
    ${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private/nonogram_solver.h
    ${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private/treasure-hunt.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/treasure-hunt/private private)

target_include_directories(treasure-hunt PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(treasure-hunt processing_engine)

# Target properties
set_target_properties(treasure-hunt PROPERTIES
                      FOLDER "examples")