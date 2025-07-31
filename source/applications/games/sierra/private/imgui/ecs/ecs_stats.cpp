#include "imgui/ecs/ecs_stats.h"
#include "imgui/ecs/ecs_stats_gauge.h"
#include "imgui/ecs/ecs_stats_counter.h"

#include "imgui/imgui.h"

namespace ppp
{
    namespace imgui
    {
        template<typename F>
        static void draw_section(const char* label, int columns, bool default_open, F draw_func)
        {
            int flags = default_open ? ImGuiTreeNodeFlags_DefaultOpen : 0;
            if (ImGui::CollapsingHeader(label, flags))
            {
                if (ImGui::BeginTable(label, columns, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInner))
                {
                    draw_func();
                    ImGui::EndTable();
                }
            }
        }

        void draw_ecs_world_stats(const ecs_world_stats_t& world_stats)
        {
            draw_section("Entities", 2, true, [&] {
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Entity Count")
                    .description("Alive entity ids in the world")
                    .gauge(&world_stats.entities.count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Not Alive")
                    .description("Not alive entity ids in the world")
                    .gauge(&world_stats.entities.not_alive_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
            });

            draw_section("Performance", 3, false, [&] {
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("FPS")
                    .description("Frames per second")
                    .gauge(&world_stats.performance.fps.gauge)
                    .time_index(world_stats.t)
                    .y_range(0, 250)
                    .ticks({ 0, 60, 120, 244 }, { "0", "60", "120", "244" })
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Frame Time")
                    .description("Time spent in frame")
                    .gauge(&world_stats.performance.frame_time.gauge)
                    .time_index(world_stats.t)
                    .y_range(0, 33)
                    .scale(100000.0f, " ms")
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("System Time")
                    .description("Time spent on running systems in frame")
                    .gauge(&world_stats.performance.system_time.gauge)
                    .time_index(world_stats.t)
                    .y_range(0, 33)
                    .scale(100000.0f, " ms")
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Emit Time")
                    .description("Time spent on notifying observers in frame")
                    .gauge(&world_stats.performance.emit_time.gauge)
                    .time_index(world_stats.t)
                    .y_range(0, 33)
                    .scale(100000.0f, " ms")
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Merge Time")
                    .description("Time spent on merging commands in frame")
                    .gauge(&world_stats.performance.merge_time.gauge)
                    .time_index(world_stats.t)
                    .y_range(0, 33)
                    .scale(10000000.0f, " ns")
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Rematch Time")
                    .description("Time spent on revalidating query caches in frame")
                    .gauge(&world_stats.performance.rematch_time.gauge)
                    .time_index(world_stats.t)
                    .y_range(0, 33)
                    .scale(1000.0f, " s")
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Delta Time")
                    .description("Time spent between each frame")
                    .gauge(&world_stats.performance.delta_time.gauge)
                    .time_index(world_stats.t)
                    .y_range(0, 33)
                    .scale(1000.0f, " ms")
                    .shaded(0.25f)
                    .build()
                );
            });

            draw_section("Commands", 3, false, [&]
            {
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Add count")
                    .description("Add commands executed")
                    .counter(&world_stats.commands.add_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Remove count")
                    .description("Remove commands executed")
                    .counter(&world_stats.commands.remove_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Delete count")
                    .description("Delete commands executed")
                    .counter(&world_stats.commands.delete_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Clear count")
                    .description("Clear commands executed")
                    .counter(&world_stats.commands.clear_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Set count")
                    .description("Set commands executed")
                    .counter(&world_stats.commands.set_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Ensure count")
                    .description("Get_mut commands executed")
                    .counter(&world_stats.commands.ensure_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Modifier count")
                    .description("Modified commands executed")
                    .counter(&world_stats.commands.modified_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Other count")
                    .description("Misc commands executed")
                    .counter(&world_stats.commands.other_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Discard count")
                    .description("Commands for already deleted entities")
                    .counter(&world_stats.commands.discard_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Batched entity count")
                    .description("Entities with batched commands")
                    .counter(&world_stats.commands.batched_entity_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Batched count")
                    .description("Number of commands batched")
                    .counter(&world_stats.commands.batched_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
            });

            draw_section("Frame", 3, false, [&] {
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Merge count")
                    .description("Number of merges (sync points)")
                    .gauge(&world_stats.frame.merge_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Pipeline build count")
                    .description("Pipeline rebuilds (happen when systems become active/enabled)")
                    .counter(&world_stats.frame.pipeline_build_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Systems ran")
                    .description("Systems ran in frame")
                    .gauge(&world_stats.frame.systems_ran.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Observers ran")
                    .description("Number of times an observer was invoked in frame")
                    .counter(&world_stats.frame.observers_ran.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Event emit count")
                    .description("Events emitted in frame")
                    .counter(&world_stats.frame.event_emit_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Remtach count")
                    .description("Number of query cache revalidations")
                    .counter(&world_stats.frame.rematch_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
            });

            draw_section("Tables", 3, false, [&] {
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Count")
                    .description("Tables in the world (including empty)")
                    .gauge(&world_stats.tables.count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Empty count")
                    .description("Empty tables in the world")
                    .gauge(&world_stats.tables.empty_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Create count")
                    .description("Number of new tables created")
                    .counter(&world_stats.tables.create_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Delete count")
                    .description("Number of tables deleted")
                    .counter(&world_stats.tables.delete_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
            });

            draw_section("Components", 3, false, [&] {
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Tag count")
                    .description("Tag ids in use")
                    .gauge(&world_stats.components.tag_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Component count")
                    .description("Component ids in use")
                    .gauge(&world_stats.components.component_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Pair count")
                    .description("Pair ids in use")
                    .gauge(&world_stats.components.pair_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Type count")
                    .description("Registered component types")
                    .gauge(&world_stats.components.type_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Create count")
                    .description("Number of new component, tag and pair ids created")
                    .counter(&world_stats.components.create_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Delete count")
                    .description("Number of component, pair and tag ids deleted")
                    .counter(&world_stats.components.delete_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
            });

            draw_section("Systems & Queries", 3, false, [&] {
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Query count")
                    .description("Queries in the world")
                    .gauge(&world_stats.queries.query_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Observer count")
                    .description("Observers in the world")
                    .gauge(&world_stats.queries.observer_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("System count")
                    .description("Systems in the world")
                    .gauge(&world_stats.queries.system_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .build()
                );
            });

            draw_section("Memory", 3, false, [&] {
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Alloc count")
                    .description("Allocations by OS API")
                    .counter(&world_stats.memory.alloc_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Realloc count")
                    .description("Reallocs by OS API")
                    .counter(&world_stats.memory.realloc_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Free count")
                    .description("Frees by OS API")
                    .counter(&world_stats.memory.free_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded()
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Outstanding alloc count")
                    .description("Outstanding allocations by OS API")
                    .gauge(&world_stats.memory.outstanding_alloc_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Block alloc count")
                    .description("Blocks allocated by block allocators")
                    .counter(&world_stats.memory.block_alloc_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Block free count")
                    .description("Blocks freed by block allocators")
                    .counter(&world_stats.memory.block_free_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Block outstanding alloc count")
                    .description("Outstanding block allocations")
                    .gauge(&world_stats.memory.block_outstanding_alloc_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Stack alloc count")
                    .description("Pages allocated by stack allocators")
                    .counter(&world_stats.memory.stack_alloc_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_counter_card(
                    counter_plot_builder()
                    .label("Stack free count")
                    .description("Pages freed by stack allocators")
                    .counter(&world_stats.memory.stack_free_count.counter)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
                ImGui::TableNextColumn();
                draw_gauge_card(
                    gauge_plot_builder()
                    .label("Stack outstanding alloc count")
                    .description("Outstanding page allocations")
                    .gauge(&world_stats.memory.stack_outstanding_alloc_count.gauge)
                    .time_index(world_stats.t)
                    .auto_fit(0.1f)
                    .shaded(0.25f)
                    .build()
                );
            });
        }
    }
}