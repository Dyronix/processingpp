#pragma once

#include "layers/layer.h"

#include "ecs/ecs_scene_manager.h"

namespace ppp 
{
    class sierra_layer : public layer
    {
    protected:
        //-------------------------------------------------------------------------
        sierra_layer(ecs::scene_manager* s, const string::string_id name = "layer"_sid, stack_order order = -1, bool always_enabled = false)
            : layer(name, order, always_enabled)
            , _scene_manager(s)
        {}

        //-------------------------------------------------------------------------
        ecs::scene_manager* scene_manager()
        {
            return _scene_manager;
        }

        //-------------------------------------------------------------------------
        const ecs::scene_manager* scene_manager() const
        {
            return _scene_manager;
        }

    private:
        ecs::scene_manager* _scene_manager;
    };
}