add_executable(agents)

target_sources(agents PRIVATE 
    ${SOURCE_EXAMPLES_DIRECTORY}/agents/private/agents.cpp)

target_include_directories(agents PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(agents processing_engine)