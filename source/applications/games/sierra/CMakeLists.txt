MESSAGE(STATUS "Adding application-sierra")
set(SIERRA_THIRDPARTY_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/third-party)
add_subdirectory(${SIERRA_THIRDPARTY_DIRECTORY}/flecs)

add_executable(sierra)

target_sources(sierra PRIVATE 
    #ecs
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_pipeline_tags.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_scene.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_scene.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_entity.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_entity.cpp
    #ecs-components
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_camera_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_camera_orbit_control_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_model_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_transform_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_shape_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_fill_color_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_components.h
    #ecs-systems
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_camera_init_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_camera_init_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_orbit_camera_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_orbit_camera_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_render_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_render_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_systems.h

    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra.cpp)

# Create the project filters
GROUPSOURCES(${SOURCE_GAMES_DIRECTORY}/sierra/private private)

# Private includes
target_include_directories(sierra PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private)
target_include_directories(sierra PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)
target_include_directories(sierra PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private)
target_include_directories(sierra PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public)
target_include_directories(sierra PRIVATE ${SIERRA_THIRDPARTY_DIRECTORY}/flecs/include)
target_include_directories(sierra PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/fmt/include)
target_include_directories(sierra PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)

target_link_libraries(sierra PRIVATE processing_engine)
target_link_libraries(sierra PRIVATE processing_framework)
target_link_libraries(sierra PRIVATE glfw)
target_link_libraries(sierra PRIVATE flecs_static)

# Include self
target_include_directories(sierra PRIVATE ${SOURCE_GAMES_DIRECTORY}/sierra/private)

# Target properties
set_target_properties(sierra PROPERTIES
                      FOLDER "games")