#include "boundary.h"

#include "color.h"
#include "shapes.h"

namespace ppp
{
    namespace raycasting
    {
        Boundary::Boundary(float x1, float y1, float x2, float y2)
        {
            m_start = glm::vec2(x1, y1);
            m_end = glm::vec2(x2, y2);
        }

        void Boundary::show() const
        {
            color::fill(255);
            shapes::line(m_start.x, m_start.y, m_end.x, m_end.y);
        }
    }
}