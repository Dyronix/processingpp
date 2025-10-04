#pragma once

#include "engine.h"
#include "sketch.h"

#include <memory>

namespace ppp
{
    /**
    * @brief User-defined function to create a new sketch
    */
    extern std::unique_ptr<sketch> make_sketch();

    /**
    * Retrieve the created sketch
    */
    sketch* get_sketch();
}