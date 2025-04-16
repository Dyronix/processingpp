#include "render/render_batch_render_strategy.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        default_batch_render_strategy::~default_batch_render_strategy() = default;

        //-------------------------------------------------------------------------
        void default_batch_render_strategy::render_batch(topology_type topology, batch_drawing_data& drawing_data) const
        {
            auto batch = drawing_data.first_batch();

            while (batch != nullptr)
            {
                batch->bind();
                batch->submit();
                batch->draw(topology);
                batch->unbind();

                batch = drawing_data.next_batch();
            }
        }
    }
}