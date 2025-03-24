#include "constants.h"

#include <glm/gtc/constants.hpp>

namespace ppp
{
    float half_pi()
    {
        return glm::half_pi<float>();
    }

    float pi()
    {
        return glm::pi<float>();
    }

    float quarter_pi()
    {
        return glm::quarter_pi<float>();
    }

    float two_pi()
    {
        return glm::two_pi<float>();
    }

    float tau()
    {
        return glm::tau<float>();
    }
}