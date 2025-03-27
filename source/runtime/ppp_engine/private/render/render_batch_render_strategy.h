#pragma once

#include "render/render_batch.h"

namespace ppp
{
    namespace render
    {
        class ibatch_render_strategy
        {
        public:
            virtual ~ibatch_render_strategy() = default;

            virtual void render_batch(topology_type topology, batch_drawing_data& drawing_data) = 0;
        };

        class default_batch_render_strategy : public ibatch_render_strategy
        {
        public:
            ~default_batch_render_strategy() override;

            void render_batch(topology_type topology, batch_drawing_data& drawing_data) override;
        };
    }
}