#pragma once

#include "render/render_instance.h"

namespace ppp
{
    namespace render
    {
        class iinstance_render_strategy
        {
        public:
            virtual ~iinstance_render_strategy() = default;

            virtual void render_instance(topology_type topology, instance_drawing_data& drawing_data) const = 0;
        };

        class default_instance_render_strategy : public iinstance_render_strategy
        {
        public:
            ~default_instance_render_strategy() override;

            void render_instance(topology_type topology, instance_drawing_data& drawing_data) const override;
        };
    }
}