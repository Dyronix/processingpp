#include "imgui/ecs/ecs_stats_gauge.h"
#include "imgui/ecs/ecs_stats_helpers.h"

#include "imgui/imgui.h"
#include "imgui/implot/implot.h"

#include <cmath>

namespace ppp
{
    namespace imgui
    {
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::label(const char* l)
        {
            _config.label = l;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::description(const char* l)
        {
            _config.label_description = l;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::gauge(const ecs_gauge_t* g)
        {
            _config.gauge = g;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::time_index(int t)
        {
            _config.time_index = t;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::y_range(float min, float max)
        {
            _config.y_min = min;
            _config.y_max = max;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::scale(float s, const char* u)
        {
            _config.scale = s;
            _config.unit = u;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::height(float h)
        {
            _config.height = h;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::ticks(const std::vector<double>& t, const std::vector<std::string>& labels)
        {
            _config.ticks = t;
            _config.tick_labels = labels;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::auto_fit(float margin)
        {
            _config.auto_fit = true;
            _config.auto_fit_margin = margin;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_builder& gauge_plot_builder::shaded(float alpha)
        {
            _config.show_shaded = true;
            _config.shaded_alpha = alpha;
            return *this;
        }
        //-------------------------------------------------------------------------
        gauge_plot_config gauge_plot_builder::build()
        {
            return _config;
        }

        //-------------------------------------------------------------------------
        void draw_gauge_card(const gauge_plot_config& config)
        {
            ImGui::BeginChild(config.label, ImVec2(0, 180), true);
            ImGui::Text("%s", config.label);
            if (config.label_description)
            {
                ImGui::SameLine();
                ImGui::Text("|");
                ImGui::SameLine();
                ImGui::Text("%s", config.label_description);
            }

            float y_min = config.y_min;
            float y_max = config.y_max;

            if (config.auto_fit && config.gauge)
            {
                float min_val = FLT_MAX;
                float max_val = -FLT_MAX;

                for (int i = 0; i < ECS_STAT_WINDOW; ++i)
                {
                    float v = config.gauge->avg[i] * config.scale;
                    min_val = std::min(min_val, v);
                    max_val = std::max(max_val, v);
                }

                constexpr float MIN_VISUAL_RANGE = 2.0f;

                if (min_val == max_val)
                {
                    float padding = max_val == 0.0f ? 1.0f : std::abs(max_val * config.auto_fit_margin);
                    y_min = std::floor(min_val - padding);
                    y_max = std::ceil(max_val + padding);
                }
                else
                {
                    float padding = (max_val - min_val) * config.auto_fit_margin;
                    y_min = std::floor(min_val - padding);
                    y_max = std::ceil(max_val + padding);

                    // Ensure at least minimum range (especially after rounding)
                    if (std::abs(y_max - y_min) < MIN_VISUAL_RANGE)
                    {
                        float center = (y_min + y_max) * 0.5f;
                        y_min = std::floor(center - MIN_VISUAL_RANGE * 0.5f);
                        y_max = std::ceil(center + MIN_VISUAL_RANGE * 0.5f);
                    }
                }
            }

            if (ImPlot::BeginPlot("##plot", ImVec2(-1, config.height), ImPlotFlags_NoLegend))
            {
                ImPlot::SetupAxisFormat(ImAxis_Y1, format_ticks_as_int, nullptr);

                int auto_fit = config.auto_fit ? ImPlotAxisFlags_AutoFit : 0;

                ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoDecorations);
                ImPlot::SetupAxis(ImAxis_Y1, nullptr);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, ECS_STAT_WINDOW);
                ImPlot::SetupAxisLimits(ImAxis_Y1, y_min, y_max, ImGuiCond_Always);

                if (config.has_custom_ticks())
                {
                    std::vector<const char*> labels_cstr;
                    for (const auto& label : config.tick_labels)
                        labels_cstr.push_back(label.c_str());
                    ImPlot::SetupAxisTicks(ImAxis_Y1, config.ticks.data(), (int)config.ticks.size(), labels_cstr.data());
                }
                else
                {
                    double mid = (y_min + y_max) * 0.5;

                    std::vector<double> tick_values = { y_min, mid, y_max };
                    std::vector<std::string> tick_labels;

                    for (double val : tick_values)
                    {
                        tick_labels.emplace_back(std::to_string(static_cast<int>(std::round(val))));
                    }

                    // Store in local copies
                    std::vector<const char*> labels_cstr;
                    for (const auto& s : tick_labels)
                    {
                        labels_cstr.push_back(s.c_str());
                    }

                    // Setup ImPlot ticks
                    ImPlot::SetupAxisTicks(ImAxis_Y1, tick_values.data(), (int)tick_values.size(), labels_cstr.data());
                }

                std::vector<float> scaled_avg(ECS_STAT_WINDOW);
                for (int i = 0; i < ECS_STAT_WINDOW; ++i)
                {
                    scaled_avg[i] = config.gauge->avg[i] * config.scale;
                }

                if (config.show_shaded)
                {
                    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, config.shaded_alpha);
                    ImPlot::PlotShaded(config.label, scaled_avg.data(), ECS_STAT_WINDOW);
                    ImPlot::PopStyleVar();
                }
                ImPlot::PlotLine(config.label, scaled_avg.data(), ECS_STAT_WINDOW);

                ImPlot::EndPlot();
            }

            float val = config.gauge->avg[config.time_index] * config.scale;
            float min = config.gauge->min[config.time_index] * config.scale;
            float max = config.gauge->max[config.time_index] * config.scale;

            ImGui::Text("Value: %.2f%s (min: %.2f, max: %.2f)", val, config.unit, min, max);

            ImGui::EndChild();
        }
    }
}