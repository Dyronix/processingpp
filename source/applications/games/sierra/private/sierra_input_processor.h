#pragma once

#include "sierra_input_event.h"

#include "util/types.h"

namespace ppp
{
    class isierra_input_processor
    {
    public:
        virtual bool on_input(const input_event& evt) = 0;
    };
}