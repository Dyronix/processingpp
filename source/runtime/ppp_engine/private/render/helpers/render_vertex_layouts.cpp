#include "render_vertex_layouts.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 1>& pos_layout()
        {
            static std::array<attribute_layout, 1> s_pos_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_format),
                    0
                }
            };

            return s_pos_layout;
        }
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 2>& pos_norm_layout()
        {
            static std::array<attribute_layout, 2> s_pos_norm_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_norm_format),
                    0
                },
                attribute_layout{
                    attribute_type::NORMAL,
                    attribute_data_type::FLOAT,

                    1,
                    3,
                    1,
                    false,
                    sizeof(pos_norm_format),
                    3 * sizeof(float)
                }
            };

            return s_pos_norm_layout;
        }
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 3>& pos_norm_col_layout()
        {
            static std::array<attribute_layout, 3> s_pos_norm_col_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_norm_color_format),
                    0
                },
                attribute_layout{
                    attribute_type::NORMAL,
                    attribute_data_type::FLOAT,

                    1,
                    3,
                    1,
                    false,
                    sizeof(pos_norm_color_format),
                    3 * sizeof(float)
                },
                attribute_layout{
                    attribute_type::COLOR,
                    attribute_data_type::FLOAT,

                    1,
                    4,
                    1,
                    false,
                    sizeof(pos_norm_color_format),
                    3 * sizeof(float) + 3 * sizeof(float)
                }
            };

            return s_pos_norm_col_layout;
        }
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 2>& pos_col_layout()
        {
            static std::array<attribute_layout, 2> s_pos_col_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_col_format),
                    0
                },
                attribute_layout{
                    attribute_type::COLOR,
                    attribute_data_type::FLOAT,

                    1,
                    4,
                    1,
                    false,
                    sizeof(pos_col_format),
                    3 * sizeof(float)
                }
            };

            return s_pos_col_layout;
        }
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 2>& pos_tex_layout()
        {
            static std::array<attribute_layout, 2> s_pos_tex_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_tex_format),
                    0
                },
                attribute_layout{
                    attribute_type::TEXCOORD,
                    attribute_data_type::FLOAT,

                    1,
                    2,
                    1,
                    false,
                    sizeof(pos_tex_format),
                    3 * sizeof(float)
                }
            };

            return s_pos_tex_layout;
        }
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 3>& pos_tex_norm_layout()
        {
            static std::array<attribute_layout, 3> s_pos_tex_norm_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_tex_norm_format),
                    0
                },
                attribute_layout{
                    attribute_type::TEXCOORD,
                    attribute_data_type::FLOAT,

                    1,
                    2,
                    1,
                    false,
                    sizeof(pos_tex_norm_format),
                    3 * sizeof(float)
                },
                attribute_layout{
                    attribute_type::NORMAL,
                    attribute_data_type::FLOAT,

                    2,
                    3,
                    1,
                    false,
                    sizeof(pos_tex_norm_format),
                    3 * sizeof(float) + 2 * sizeof(float)
                }
            };

            return s_pos_tex_norm_layout;
        }
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 4>& pos_tex_col_layout()
        {
            static std::array<attribute_layout, 4> s_pos_tex_col_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_tex_col_format),
                    0
                },
                attribute_layout{
                    attribute_type::TEXCOORD,
                    attribute_data_type::FLOAT,

                    1,
                    2,
                    1,
                    false,
                    sizeof(pos_tex_col_format),
                    3 * sizeof(float)
                },
                attribute_layout{
                    attribute_type::COLOR,
                    attribute_data_type::FLOAT,

                    2,
                    4,
                    1,
                    false,
                    sizeof(pos_tex_col_format),
                    3 * sizeof(float) + 2 * sizeof(float)
                },
                attribute_layout{
                    attribute_type::MATERIAL_INDEX,
                    attribute_data_type::INT,

                    3,
                    1,
                    1,
                    false,
                    sizeof(pos_tex_col_format),
                    3 * sizeof(float) + 2 * sizeof(float) + 4 * sizeof(float)
                }
            };
            
            return s_pos_tex_col_layout;
        }
        //-------------------------------------------------------------------------
        const std::array<attribute_layout, 5>& pos_tex_col_norm_layout()
        {
            static std::array<attribute_layout, 5> s_pos_tex_col_norm_layout
            {
                attribute_layout{
                    attribute_type::POSITION,
                    attribute_data_type::FLOAT,

                    0,
                    3,
                    1,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    0
                },
                attribute_layout{
                    attribute_type::TEXCOORD,
                    attribute_data_type::FLOAT,

                    1,
                    2,
                    1,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float)
                },
                attribute_layout{
                    attribute_type::NORMAL,
                    attribute_data_type::FLOAT,

                    2,
                    3,
                    1,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float) + 2 * sizeof(float)
                },
                attribute_layout{
                    attribute_type::COLOR,
                    attribute_data_type::FLOAT,

                    3,
                    4,
                    1,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float)
                },
                attribute_layout{
                    attribute_type::MATERIAL_INDEX,
                    attribute_data_type::INT,

                    4,
                    1,
                    1,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float) + 4 * sizeof(float)
                }
            };

            return s_pos_tex_col_norm_layout;
        }

        //-------------------------------------------------------------------------
        const attribute_layout* fill_user_layout(vertex_type type)
        {
            switch (type)
            {
            case vertex_type::POSITION: return pos_layout().data();
            case vertex_type::POSITION_NORMAL: return pos_norm_layout().data();
            case vertex_type::POSITION_NORMAL_COLOR: return pos_norm_col_layout().data();
            case vertex_type::POSITION_TEXCOORD: return pos_tex_layout().data();
            case vertex_type::POSITION_TEXCOORD_NORMAL: return pos_tex_norm_layout().data();
            case vertex_type::POSITION_COLOR: return pos_col_layout().data();
            case vertex_type::POSITION_TEXCOORD_COLOR: return pos_tex_col_layout().data();
            case vertex_type::POSITION_TEXCOORD_NORMAL_COLOR: return pos_tex_col_norm_layout().data();
            }

            log::error("Unsupported vertex type");
            return nullptr;
        }

        //-------------------------------------------------------------------------
        u64 fill_user_layout_count(vertex_type type)
        {
            switch (type)
            {
            case vertex_type::POSITION: return pos_layout().size();
            case vertex_type::POSITION_NORMAL: return pos_norm_layout().size();
            case vertex_type::POSITION_NORMAL_COLOR: return pos_norm_col_layout().size();
            case vertex_type::POSITION_TEXCOORD: return pos_tex_layout().size();
            case vertex_type::POSITION_TEXCOORD_NORMAL: return pos_tex_norm_layout().size();
            case vertex_type::POSITION_COLOR: return pos_col_layout().size();
            case vertex_type::POSITION_TEXCOORD_COLOR: return pos_tex_col_layout().size();
            case vertex_type::POSITION_TEXCOORD_NORMAL_COLOR: return pos_tex_col_norm_layout().size();
            }

            log::error("Unsupported vertex type");
            return 0;
        }
    }
}