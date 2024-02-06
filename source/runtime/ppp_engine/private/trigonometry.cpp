#include "trigonometry.h"

namespace ppp
{
    namespace trigonometry
    {
        namespace internal
        {
            AngleMode _angle_mode = AngleMode::RADIANS;
        }

        void angle_mode(AngleMode mode)
        {
            internal::_angle_mode = mode;
        }

        AngleMode angle_mode()
        {
            return internal::_angle_mode;
        }
    }
}