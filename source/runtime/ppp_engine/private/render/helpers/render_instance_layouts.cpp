#include "render/helpers/render_instance_layouts.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 2>& color_world_layout()
        {
            static std::array<attribute_layout, 2> s_color_world_layout
            {
                attribute_layout{
                    attribute_type::WORLD_MATRIX,
                    attribute_data_type::FLOAT,

                    0,
                    4,
                    4,
                    false,
                    sizeof(world_color_format),
                    0
                },
                attribute_layout{
                    attribute_type::COLOR,
                    attribute_data_type::FLOAT,

                    1,
                    4,
                    1,
                    false,
                    sizeof(world_color_format),
                    4 * 4 * sizeof(float)
                }
            };

            return s_color_world_layout;
        }

        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 3>& color_world_matid_layout()
        {
            static std::array<attribute_layout, 3> s_color_world_matid_layout
            {
                attribute_layout{
                    attribute_type::MATERIAL_INDEX,
                    attribute_data_type::INT,

                    0,
                    1,
                    1,
                    false,
                    sizeof(world_color_matid_format),
                    0
                },
                attribute_layout{
                    attribute_type::WORLD_MATRIX,
                    attribute_data_type::FLOAT,

                    1,
                    4,
                    4,
                    false,
                    sizeof(world_color_matid_format),
                    sizeof(s32)
                },
                attribute_layout{
                    attribute_type::COLOR,
                    attribute_data_type::FLOAT,

                    2,
                    4,
                    1,
                    false,
                    sizeof(world_color_matid_format),
                    sizeof(s32) + 4 * 4 * sizeof(float)
                }
            };
            
            return s_color_world_matid_layout;
        }
    }
}