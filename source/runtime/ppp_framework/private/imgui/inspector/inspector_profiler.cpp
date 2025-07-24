#include "imgui/inspector/inspector_profiler.h"

#include "imgui/imgui.h"
#include "imgui/implot/implot.h"

#include "util/types.h"
#include "util/profiler.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            void inspect_profiler(bool& show)
            {
                ImGui::Begin("Profiler", &show, ImGuiWindowFlags_NoCollapse);

                for (auto& itr : profiler::ctx().times)
                {
                    auto& e = itr.second;

                    f32 duration = (f32)((f64)e.accum.count() / 1000000.0);
                    if (e.history.size() > 100)
                    {
                        e.history.pop_front();
                    }
                    e.history.push_back(duration);

                    e.avg = 0.0f;
                    for (f32 f : e.history)
                    {
                        e.avg += f;
                    }

                    e.avg /= (f32)e.history.size();
                }

                if (ImPlot::BeginPlot("Profiler", ImVec2(-1, 200)))
                {
                    ImPlot::SetupAxes("Sample", "Time");
                    ImPlot::SetupAxesLimits(0, 50, 0, 20);
                    for (auto& itr : profiler::ctx().times)
                    {
                        auto& e = itr.second;

                        std::vector<f32> vals(e.history.begin(), e.history.end());

                        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                        ImPlot::PlotShaded(itr.first.c_str(), vals.data(), (s32)vals.size());
                        ImPlot::PopStyleVar();
                        ImPlot::PlotLine(itr.first.c_str(), vals.data(), (s32)vals.size());
                    }
                    ImPlot::EndPlot();
                }

                for (auto& itr : profiler::ctx().times)
                {
                    ImGui::LabelText(itr.first.c_str(), "%fms count:%d", itr.second.avg, itr.second.count);
                }

                ImGui::End();

                for (auto& itr : profiler::ctx().times)
                {
                    itr.second.accum = {};
                    itr.second.count = 0;
                }
            }
        }
    }
}


