#include "sierra_imgui_layer.h"
#include "sierra_engine_context.h"

#include "imgui/sierra_inspector.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_imgui_layer::sierra_imgui_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "imgui"_sid, 999, true)
    {}

    //-------------------------------------------------------------------------
    void sierra_imgui_layer::on_attached()
    {
        // Nothing to implement
    }
    //-------------------------------------------------------------------------
    void sierra_imgui_layer::on_detached()
    {
        // Nothing to implement
    }

    //-------------------------------------------------------------------------
    void sierra_imgui_layer::on_inspector_draw()
    {
        imgui::draw_inspector(context());
    }
}