#pragma once

#include <glm/glm.hpp>

namespace ppp
{
    namespace raycasting
    {
        class Boundary
        {
        public:
            Boundary(float x1, float y1, float x2, float y2);

            void show() const;

            const glm::vec2& start() const { return m_start; }
            const glm::vec2& end() const { return m_end; }

        private:
            glm::vec2 m_start;
            glm::vec2 m_end;
        };
    }
}