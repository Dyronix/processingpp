add_executable(a-star)

target_sources(a-star PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-grid.h
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-grid-cell.h
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-grid-cell.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-grid-drawer.h
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-grid-iterator.h
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-grid-iterator.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-path-node.h
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-path-node.cpp
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-path-node-costs.h
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-path-resolver.h
    ${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private/a-star-path-tracer.h)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/engine/a-star/private private)

target_include_directories(a-star PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(a-star processing_engine)

# Target properties
set_target_properties(a-star PROPERTIES
                      FOLDER "examples/engine")