#pragma once

#include <flecs.h>

#include <vector>
#include <string>

namespace ppp
{
    namespace imgui
    {
        struct counter_plot_config
        {
            const char* label = nullptr;
            const char* label_description = nullptr;
            const ecs_counter_t* counter = nullptr;
            int time_index = 0;

            float y_min = 0.0f;
            float y_max = 1.0f;

            float scale = 1.0f;
            const char* unit = "";

            float height = 100.0f;

            std::vector<double> ticks;
            std::vector<std::string> tick_labels;

            bool auto_fit = false;
            float auto_fit_margin = 0.1f;

            bool show_shaded = false;
            float shaded_alpha = 0.25f;

            bool has_custom_ticks() const
            {
                return ticks.size() == tick_labels.size() && !ticks.empty();
            }
        };


        class counter_plot_builder
        {
            counter_plot_config _config;

        public:
            counter_plot_builder& label(const char* l);
            counter_plot_builder& description(const char* l);
            counter_plot_builder& counter(const ecs_counter_t* c);
            counter_plot_builder& time_index(int t);
            counter_plot_builder& y_range(float min, float max);
            counter_plot_builder& scale(float s, const char* u = "");
            counter_plot_builder& height(float h);
            counter_plot_builder& ticks(const std::vector<double>& t, const std::vector<std::string>& labels);
            counter_plot_builder& auto_fit(float margin = 0.1f);
            counter_plot_builder& shaded(float alpha = 0.25f);
            counter_plot_config build();
        };

        void draw_counter_card(const counter_plot_config& config);
    }
}