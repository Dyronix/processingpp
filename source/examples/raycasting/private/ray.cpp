#include "ray.h"

#include "color.h"
#include "shapes.h"
#include "trigonometry.h"

namespace ppp
{
    namespace raycasting
    {
        namespace internal
        {
            constexpr float _display_length = 1.0f;
        }

        Ray::Ray(float posx, float posy, float angle)
        {
            m_position = glm::vec2(posx, posy);
            m_direction.x = trigonometry::cos(angle);
            m_direction.y = trigonometry::sin(angle);

            m_length = internal::_display_length;
        }

        void Ray::lookat(float x, float y)
        {
            m_direction.x = x - m_position.x;
            m_direction.y = y - m_position.y;

            m_direction = glm::normalize(m_direction);
        }

        void Ray::set_direction(float x, float y)
        {
            m_direction.x = x;
            m_direction.y = y;
        }

        void Ray::set_position(float x, float y)
        {
            m_position.x = x;
            m_position.y = y;
        }

        void Ray::show() const
        {
            color::fill(255);
            shapes::line(m_position.x, m_position.y, m_position.x + (m_direction.x * m_length), m_position.y + (m_direction.y * m_length));
        }

        RaycastHit Ray::cast(float x1, float y1, float x2, float y2) const
        {
            const float x3 = m_position.x;
            const float y3 = m_position.y;
            const float x4 = m_position.x + m_direction.x;
            const float y4 = m_position.y + m_direction.y;

            const float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (den == 0)
            {
                return RaycastHit{ -1.0f, {}, false };
            }

            const float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
            const float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

            if (t > 0 && t < 1 && u > 0)
            {
                RaycastHit hit;

                hit.t = t;
                hit.point.x = x1 + t * (x2 - x1);
                hit.point.y = y1 + t * (y2 - y1);
                hit.point.z = 0.0f;
                hit.hit = true;

                return hit;
            }

            return RaycastHit{ -1.0f, {}, false };
        }
    }
}