#include "render/render_instance_render_strategy.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        default_instance_render_strategy::~default_instance_render_strategy() = default;

        //-------------------------------------------------------------------------
        void default_instance_render_strategy::render_instance(topology_type topology, instance_drawing_data& drawing_data) const
        {
            auto inst = drawing_data.first_instance();

            if (inst != nullptr)
            {
                while (inst != nullptr)
                {
                    inst->bind();
                    inst->submit();
                    inst->draw(topology);
                    inst->unbind();

                    inst = drawing_data.next_instance();
                }
            }
        }
    }
}
