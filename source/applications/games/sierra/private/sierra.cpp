#include "framework.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_icons_font_awesome.h"
#include "imgui/implot/implot.h"

#include "ecs/ecs_scene.h"
#include "ecs/components/ecs_components.h"

#include <flecs/addons/stats.h>

namespace
{
    template<typename F>
    static void draw_section(const char* label, int columns, F draw_func)
    {
        ImGui::SeparatorText(label);
        if (ImGui::BeginTable(label, columns, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInner)) 
        {
            draw_func();
            ImGui::EndTable();
        }
    }

    //-------------------------------------------------------------------------
    static void tooltip(const char* tooltip)
    {
        if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.6f)
        {
            ImGui::BeginTooltip();
            ImGui::SetTooltip("%s", tooltip);
            ImGui::EndTooltip();
        }
    }

    //static void draw_gauge_card_implot(const char* label, const ecs_gauge_t& gauge, int t)
    //{
    //    ImGui::Text("%s: %.4f (min: %.4f, max: %.4f)",
    //        label,
    //        gauge.avg[t],
    //        gauge.min[t],
    //        gauge.max[t]);
    //}

    //static void draw_gauge_card_implot(const char* label, const ecs_gauge_t& gauge, int t)
    //{
    //    ImGui::PushID(label); // ensure uniqueness
    //
    //    if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen))
    //    {
    //        ImGui::Text("Current: %.4f", gauge.avg[t]);
    //        ImGui::Text("Min:     %.4f", gauge.min[t]);
    //        ImGui::Text("Max:     %.4f", gauge.max[t]);
    //
    //        // Optional: Plot average gauge values over time
    //        ImGui::SeparatorText("History (avg)");
    //        ImGui::PlotLines("##avg_plot", gauge.avg, ECS_STAT_WINDOW, t + 1, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 60));
    //        ImGui::PlotLines("##min_plot", gauge.min, ECS_STAT_WINDOW, t + 1, "min", FLT_MAX, FLT_MAX, ImVec2(0, 40));
    //        ImGui::PlotLines("##max_plot", gauge.max, ECS_STAT_WINDOW, t + 1, "max", FLT_MAX, FLT_MAX, ImVec2(0, 40));
    //    }
    //
    //    ImGui::PopID();
    //}

    struct gauge_plot_config 
    {
        const char* label = nullptr;
        const ecs_gauge_t* gauge = nullptr;
        int time_index = 0;

        float y_min = 0.0f;
        float y_max = 1.0f;

        float scale = 1.0f;
        const char* unit = "";

        float height = 100.0f;

        std::vector<double> ticks;
        std::vector<std::string> tick_labels;

        bool auto_fit = false;

        bool has_custom_ticks() const 
        {
            return ticks.size() == tick_labels.size() && !ticks.empty();
        }
    };

    class gauge_plot_builder 
    {
        gauge_plot_config config_;

    public:
        gauge_plot_builder& label(const char* l) 
        {
            config_.label = l;
            return *this;
        }

        gauge_plot_builder& gauge(const ecs_gauge_t* g) 
        {
            config_.gauge = g;
            return *this;
        }

        gauge_plot_builder& time_index(int t) 
        {
            config_.time_index = t;
            return *this;
        }

        gauge_plot_builder& y_range(float min, float max) 
        {
            config_.y_min = min;
            config_.y_max = max;
            return *this;
        }

        gauge_plot_builder& scale(float s, const char* u = "") 
        {
            config_.scale = s;
            config_.unit = u;
            return *this;
        }

        gauge_plot_builder& height(float h) 
        {
            config_.height = h;
            return *this;
        }

        gauge_plot_builder& ticks(const std::vector<double>& t, const std::vector<std::string>& labels) 
        {
            config_.ticks = t;
            config_.tick_labels = labels;
            return *this;
        }

        gauge_plot_builder& auto_fit()
        {
            config_.auto_fit = true;
        }

        gauge_plot_config build() 
        {
            return config_;
        }
    };

    void draw_gauge_card(const gauge_plot_config& config)
    {
        ImGui::BeginChild(config.label, ImVec2(0, 180), true);
        ImGui::Text("%s", config.label);

        if (ImPlot::BeginPlot("##plot", ImVec2(-1, config.height), ImPlotFlags_NoLegend)) {
            ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoDecorations);
            ImPlot::SetupAxis(ImAxis_Y1, nullptr);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, ECS_STAT_WINDOW);
            ImPlot::SetupAxisLimits(ImAxis_Y1, config.y_min, config.y_max);

            if (config.has_custom_ticks()) {
                std::vector<const char*> labels_cstr;
                for (const auto& label : config.tick_labels)
                    labels_cstr.push_back(label.c_str());
                ImPlot::SetupAxisTicks(ImAxis_Y1, config.ticks.data(), (int)config.ticks.size(), labels_cstr.data());
            }

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);

            std::vector<float> scaled_avg(ECS_STAT_WINDOW);
            for (int i = 0; i < ECS_STAT_WINDOW; ++i)
                scaled_avg[i] = config.gauge->avg[i] * config.scale;

            ImPlot::PlotShaded(config.label, scaled_avg.data(), ECS_STAT_WINDOW);
            ImPlot::PopStyleVar();
            ImPlot::PlotLine(config.label, scaled_avg.data(), ECS_STAT_WINDOW);

            ImPlot::EndPlot();
        }

        float val = config.gauge->avg[config.time_index] * config.scale;
        float min = config.gauge->min[config.time_index] * config.scale;
        float max = config.gauge->max[config.time_index] * config.scale;

        ImGui::Text("Value: %.2f%s (min: %.2f, max: %.2f)", val, config.unit, min, max);

        ImGui::EndChild();
    }

    //static void draw_counter_card_implot(const char* label, const ecs_counter_t& counter, int t)
    //{
    //    ImGui::Text("%s: %.0f (rate: %.2f)",
    //        label,
    //        counter.value[t],
    //        counter.rate.avg[t]);
    //}

    //static void draw_counter_card_implot(const char* label, const ecs_counter_t& counter, int t)
    //{
    //    ImGui::PushID(label);
    //
    //    if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen))
    //    {
    //        ImGui::Text("Value: %.0f", counter.value[t]);
    //        ImGui::Text("Rate:  %.2f", counter.rate.avg[t]);
    //
    //        ImGui::SeparatorText("Rate History");
    //        ImGui::PlotLines("##rate_plot", counter.rate.avg, ECS_STAT_WINDOW, t + 1, nullptr, 0.0f, FLT_MAX, ImVec2(0, 60));
    //    }
    //
    //    ImGui::PopID();
    //}

    void draw_counter_card_implot(const char* label, const ecs_counter_t& counter, int t)
    {
        ImGui::BeginChild(label, ImVec2(0, 180), true);
        ImGui::Text("%s", label);

        if (ImPlot::BeginPlot("##counter_plot", ImVec2(-1, 100))) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_NoTickLabels);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, ECS_STAT_WINDOW, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, FLT_MAX, ImGuiCond_Always);
            ImPlot::PlotLine("Value", counter.value, ECS_STAT_WINDOW, 1, 0, sizeof(float));
            ImPlot::PlotLine("Rate", counter.rate.avg, ECS_STAT_WINDOW, 1, 0, sizeof(float));
            ImPlot::EndPlot();
        }

        ImGui::Text("Value: %.0f | Rate: %.2f", counter.value[t], counter.rate.avg[t]);

        ImGui::EndChild();
    }

    static void draw_stats(const ecs_world_stats_t& world_stats)
    {
        draw_section("Entities", 2, [&] {
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("Entity Count")
                .gauge(&world_stats.entities.count.gauge)
                .time_index(world_stats.t)
                .y_range(0, INT_MAX)
                .auto_fit()
                .build()
            );
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("FPS")
                .gauge(&world_stats.entities.not_alive_count.gauge)
                .time_index(world_stats.t)
                .y_range(0, INT_MAX)
                .build()
            );
            draw_gauge_card("Not Alive", world_stats.entities.not_alive_count.gauge, world_stats.t);
        });

        draw_section("Performance", 3, [&] {
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("FPS")
                .gauge(&world_stats.performance.fps.gauge)
                .time_index(world_stats.t)
                .y_range(0, 250)
                .ticks({ 0, 60, 120, 244 }, { "0", "60", "120", "244" })
                .build()
            );
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("Frame Time")
                .gauge(&world_stats.performance.frame_time.gauge)
                .time_index(world_stats.t)
                .y_range(0, 33)
                .scale(100000.0f, " ms")
                .build()
            );
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("System Time")
                .gauge(&world_stats.performance.system_time.gauge)
                .time_index(world_stats.t)
                .y_range(0, 33)
                .scale(100000.0f, " ms")
                .build()
            );
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("Merge Time")
                .gauge(&world_stats.performance.merge_time.gauge)
                .time_index(world_stats.t)
                .y_range(0, 33)
                .scale(10000000.0f, " ns")
                .build()
            );
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("Rematch Time")
                .gauge(&world_stats.performance.rematch_time.gauge)
                .time_index(world_stats.t)
                .y_range(0, 33)
                .scale(1000.0f, " s")
                .build()
            );
            ImGui::TableNextColumn(); 
            draw_gauge_card(
                gauge_plot_builder()
                .label("Delta Time")
                .gauge(&world_stats.performance.delta_time.gauge)
                .time_index(world_stats.t)
                .y_range(0, 33)
                .scale(1000.0f, " ms")
                .build()
            );
        });

        //draw_section("Commands", 3, [&] {
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Add", world_stats.commands.add_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Remove", world_stats.commands.remove_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Delete", world_stats.commands.delete_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Clear", world_stats.commands.clear_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Set", world_stats.commands.set_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Ensure", world_stats.commands.ensure_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Modified", world_stats.commands.modified_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Other", world_stats.commands.other_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Discard", world_stats.commands.discard_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Batched Count", world_stats.commands.batched_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Batched Entities", world_stats.commands.batched_entity_count.counter, world_stats.t);
        //});

        //draw_section("Frames", 3, [&] {
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Merges", world_stats.frame.merge_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Pipeline Rebuilds", world_stats.frame.pipeline_build_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Systems Ran", world_stats.frame.systems_ran.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Observers Invoked", world_stats.frame.observers_ran.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Events Emitted", world_stats.frame.event_emit_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Query Revalidations", world_stats.frame.rematch_count.counter, world_stats.t);
        //});

        //draw_section("Tables", 3, [&] {
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Table Count", world_stats.tables.count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Empty Tables", world_stats.tables.empty_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Tables Created", world_stats.tables.create_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Tables Deleted", world_stats.tables.delete_count.counter, world_stats.t);
        //});

        //draw_section("Components", 3, [&] {
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Tags", world_stats.components.tag_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Components", world_stats.components.component_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Pairs", world_stats.components.pair_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Types", world_stats.components.type_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Created", world_stats.components.create_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Deleted", world_stats.components.delete_count.counter, world_stats.t);
        //});

        //draw_section("Systems & Queries", 3, [&] {
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Systems", world_stats.queries.system_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Queries", world_stats.queries.query_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Observers", world_stats.queries.observer_count.gauge, world_stats.t);
        //});

        //draw_section("Memory", 3, [&] {
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Allocs", world_stats.memory.alloc_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Reallocs", world_stats.memory.realloc_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Frees", world_stats.memory.free_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Outstanding Allocs", world_stats.memory.outstanding_alloc_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Block Allocs", world_stats.memory.block_alloc_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Block Frees", world_stats.memory.block_free_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Outstanding Blocks", world_stats.memory.block_outstanding_alloc_count.gauge, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Stack Page Allocs", world_stats.memory.stack_alloc_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_counter_card_implot("Stack Page Frees", world_stats.memory.stack_free_count.counter, world_stats.t);
        //    ImGui::TableNextColumn(); draw_gauge_card_implot("Outstanding Stack Pages", world_stats.memory.stack_outstanding_alloc_count.gauge, world_stats.t);
        //});
    }

}

namespace ppp
{
    struct sierra_inspector_context
    {
        bool show_ecs_data;
    };

    sierra_inspector_context& ctx()
    {
        static sierra_inspector_context context;

        return context;
    }

    class sierra : public sketch
    {
    public:
        sierra() = default;
        ~sierra() override = default;

    public:
        void setup() override;
        void tick(float dt) override;
        void draw() override;
        void inspector_draw() override;

    private:
        int _window_width = 1280;
        int _window_height = 720;

        ecs::scene _scene;
    };

    //-------------------------------------------------------------------------
    void sierra::setup()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        shader(material::tags::unlit::color());

        enable_batched_draw_mode();
        enable_wireframe_mode(true);
        enable_solid_mode(true);

        wireframe_color(0.0f, 0.0f, 0.0f);
        wireframe_linewidth(2.0f);

        // Make Box
        {
            auto e_box = _scene.create_entity("box");

            e_box.set<ecs::transform_component>({
                {0, 0, 0},                          /*.position */
                {1.0f, 1.0f, 1.0f},                 /*.scale */
                glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
                });
            e_box.set<ecs::shape_component>({
                []() { ppp::box(50.0f, 50.0f, 50.0f); } /*.draw_fn */
                });
            e_box.set<ecs::fill_color_component>({
                255,    /*.red */
                0,      /*.green */
                0,      /*.blue */
                255     /*.alpha */
                });
        }

        // Make Camera
        {
            auto e_camera = _scene.create_entity("orbit_camera");

            e_camera.set<ecs::transform_component>({
                {20, -40, 400},                     /*.position */
                {1.0f, 1.0f, 1.0f},                 /*.scale */
                glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
                });
            e_camera.set<ecs::camera_component>({
                "",                                         /*.tag */
                ecs::projection_type::PERSPECTIVE,          /*.type */
                55.0f,                                      /*.fovy */
                (f32)_window_width / (f32)_window_height,   /*.aspect_ratio */
                0.1f,                                       /*.near_clip */
                2000.0f                                     /*.far_clip */
                });
            e_camera.set<ecs::orbit_control_component>({
                200.0f, /*.zoom_sensitivity */
                0.5f,   /*.panning_sensitivity */
                0.5f,   /*.rotation_sensitivity */
                1.0f,   /*.min_zoom */
                600.0f  /*.max_zoom */
                });
        }

        _scene.init();
    }

    //-------------------------------------------------------------------------
    void sierra::tick(float dt)
    {
        _scene.tick(dt);
    }

    //-------------------------------------------------------------------------
    void sierra::draw()
    {
        background(200);
        
        _scene.draw();
    }

    //-------------------------------------------------------------------------
    void sierra::inspector_draw()
    {
        // Data
        {
            if (ImGui::Button(ICON_FA_DATABASE))
            {
                ctx().show_ecs_data = true;
            }
            tooltip("ECS");
        }

        if (ctx().show_ecs_data)
        {
            ImGui::Begin("ECS", &ctx().show_ecs_data);

            ImGui::SeparatorText("World Stats");
            draw_stats(_scene.tick_stats());
            //ImGui::SeparatorText("Draw Stats");
            //draw_stats(_scene.draw_stats());
            ImGui::End();
        }

        static bool _first = true;
        if (_first)
        {
            auto q = _scene.world().query_builder().build();
            int entity_count = 0;

            q.each([&](flecs::entity) {
                ++entity_count;
            });

            std::cout << "Total live entities: " << entity_count << "\n";

            _first = false;
        }
    }

    //-------------------------------------------------------------------------
    std::unique_ptr<sketch> make_sketch()
    {
        return std::make_unique<sierra>();
    }
}