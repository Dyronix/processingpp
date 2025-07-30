#pragma once

#include <flecs.h>

#include <vector>
#include <string>

namespace ppp
{
    namespace imgui
    {
        struct gauge_plot_config
        {
            const char* label = nullptr;
            const char* label_description = nullptr;
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
            float auto_fit_margin = 0.1f;

            bool show_shaded = false;
            float shaded_alpha = 0.25f;

            bool has_custom_ticks() const
            {
                return ticks.size() == tick_labels.size() && !ticks.empty();
            }
        };

        class gauge_plot_builder
        {
        public:
            gauge_plot_builder& label(const char* l);
            gauge_plot_builder& description(const char* l);
            gauge_plot_builder& gauge(const ecs_gauge_t* g);
            gauge_plot_builder& time_index(int t);
            gauge_plot_builder& y_range(float min, float max);
            gauge_plot_builder& scale(float s, const char* u = "");
            gauge_plot_builder& height(float h);
            gauge_plot_builder& ticks(const std::vector<double>& t, const std::vector<std::string>& labels);
            gauge_plot_builder& auto_fit(float margin = 0.1f);
            gauge_plot_builder& shaded(float alpha = 0.25f);
            gauge_plot_config build();

        private:
            gauge_plot_config _config;
        };

        void draw_gauge_card(const gauge_plot_config& config);
    }
}