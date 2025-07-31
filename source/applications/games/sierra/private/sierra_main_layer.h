#pragma once

#include "sierra_layer.h"

namespace ppp
{
    class sierra_main_layer : public sierra_layer
    {
    public:
        sierra_main_layer(sierra_engine_context* ctx);

    protected:
        void on_enable() override;
    };
}