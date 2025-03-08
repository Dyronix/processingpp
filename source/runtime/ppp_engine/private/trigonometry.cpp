#include "trigonometry.h"

namespace ppp
{
    namespace trigonometry
    {
        namespace internal
        {
            angle_mode_type _angle_mode = angle_mode_type::RADIANS;
        }

        void angle_mode(angle_mode_type mode)
        {
            internal::_angle_mode = mode;
        }

        angle_mode_type angle_mode()
        {
            return internal::_angle_mode;
        }
    }
}