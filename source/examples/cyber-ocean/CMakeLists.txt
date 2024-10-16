add_executable(cyber-ocean)

target_sources(cyber-ocean PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/cyber-ocean/private/cyber-ocean.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_EXAMPLES_DIRECTORY}/cyber-ocean/private private)

target_include_directories(cyber-ocean PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(cyber-ocean processing_engine)

# Target properties
set_target_properties(cyber-ocean PROPERTIES
                      FOLDER "examples")