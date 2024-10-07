#include "util/color_ops.h"

namespace ppp
{
    namespace color
    {
        u32 convert_color(const glm::vec4 color)
        {
            u32 out;
            out = (conversions::f32_to_uint8(color.x)) << 24;
            out |= (conversions::f32_to_uint8(color.y)) << 16;
            out |= (conversions::f32_to_uint8(color.z)) << 8;
            out |= (conversions::f32_to_uint8(color.w)) << 0;
            return out;
        }

        glm::vec4 convert_color(u32 color)
        {
            f32 scale = 1.0f / 255.0f;
            return glm::vec4(
                ((color >> 24) & 0xFF) * scale,
                ((color >> 16) & 0xFF) * scale,
                ((color >> 8) & 0xFF) * scale,
                ((color >> 0) & 0xFF) * scale);
        }
    }
}