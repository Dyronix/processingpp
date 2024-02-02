#pragma once

#include "ray.h"
#include "boundary.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace raycasting
    {
        class Particle
        {
        public:
            Particle(float x, float y);

            void set_position(float x, float y);

            void look(const Boundary& bound);
            void look(const std::vector<Boundary>& bounds);

            void show();

        private:
            glm::vec2 m_position;
            
            std::vector<Ray> m_rays;
        };
    }
}