#include "sierra_ui_layer.h"

#include "util/log.h"

#include "shapes.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_ui_layer::sierra_ui_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "ui"_sid, 500, false)
    {
    }

    //-------------------------------------------------------------------------
    void sierra_ui_layer::on_attached()
    {
        // Nothing to implement
    }
    //-------------------------------------------------------------------------
    void sierra_ui_layer::on_enable()
    {
        // Nothing to implement
    }
}