MESSAGE(STATUS "Adding application-sierra")
set(SIERRA_THIRDPARTY_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/third-party)
add_subdirectory(${SIERRA_THIRDPARTY_DIRECTORY}/flecs)

add_executable(sierra)

target_sources(sierra PRIVATE 
    #imgui
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/sierra_inspector.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/sierra_inspector.cpp
    #imgui-layers
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/layers/layer_stats.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/layers/layer_stats.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/layers/layer_floating_window.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/layers/layer_floating_window.cpp
    #imgui-ecs
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats_counter.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats_counter.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats_gauge.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats_gauge.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats_helpers.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/imgui/ecs/ecs_stats_helpers.cpp
    #ecs
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_pipeline_tags.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_scene.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_scene.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_scene_manager.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_scene_manager.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_entity.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/ecs_entity.cpp
    #ecs-components
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_placement_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_bounding_box_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_bullet_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_bullet_state.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_camera_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_camera_orbit_control_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_enemy_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_enemy_goal_trigger_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_enemy_state.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_model_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_transform_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_shape_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_fill_color_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_pickable_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_picked_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_tile_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_tower_component.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_tower_state.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/components/ecs_components.h
    #ecs-systems
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_placement_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_placement_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_camera_init_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_camera_init_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_orbit_camera_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_orbit_camera_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_render_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_render_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_picking_system.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_picking_system.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/ecs/systems/ecs_systems.h
    #grid
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-grid.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-grid-cell.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-grid-cell.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-grid-drawer.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-grid-iterator.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-grid-iterator.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-path-node.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-path-node.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-path-node-costs.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-path-resolver.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/grid/a-star-path-tracer.h
    #layers
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/layers/layer.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/layers/layer.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/layers/layer_stack.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/layers/layer_stack.cpp

    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_engine_context.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_player_state.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_player_state.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_input_handler.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_input_handler.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_placement_layer.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_placement_layer.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_level_layer.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_level_layer.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_layer.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_layer.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_imgui_layer.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_imgui_layer.cpp
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_main_layer.h
    ${SOURCE_GAMES_DIRECTORY}/sierra/private/sierra_main_layer.cpp
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