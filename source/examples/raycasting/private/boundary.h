#pragma once

#include "vector.h"

namespace ppp
{
    namespace raycasting
    {
        class Boundary
        {
        public:
            Boundary(float x1, float y1, float x2, float y2);

            void show() const;

            const vec2& start() const { return m_start; }
            const vec2& end() const { return m_end; }

        private:
            vec2 m_start;
            vec2 m_end;
        };
    }
}