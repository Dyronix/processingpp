#include "particle.h"

#include "constants.h"
#include "trigonometry.h"
#include "color.h"
#include "shapes.h"

namespace ppp
{
    namespace raycasting
    {
        Particle::Particle(float x, float y)
        {
            m_position.x = x;
            m_position.y = y;

            for (float i = 0; i <= two_pi(); i += radians(1.0f))
            {
                Ray r = Ray(x, y, i);

                m_rays.push_back(r);
            }
        }

        void Particle::set_position(float x, float y)
        {
            m_position.x = x;
            m_position.y = y;

            for (Ray& r : m_rays)
            {
                r.set_position(x, y);
            }
        }

        void Particle::look(const Boundary& b)
        {
            fill(255);
            for (const Ray& r : m_rays)
            {
                RaycastHit hit = r.cast(b.start().x, b.start().y, b.end().x, b.end().y);
                if (hit)
                {
                    line(m_position.x, m_position.y, hit.point.x, hit.point.y);
                }
            }
        }

        void Particle::look(const std::vector<Boundary>& bounds)
        {
            for (const Ray& r : m_rays)
            {
                float record = 1e8;
                vec3 closest_point = vec3(0.0f, 0.0f, 0.0f);

                for (const Boundary& b : bounds)
                {
                    const RaycastHit r_hit = r.cast(b.start().x, b.start().y, b.end().x, b.end().y);
                    if (r_hit)
                    {
                        const float dist = vec3(m_position.x, m_position.y, 0.0f).dist(r_hit.point);
                        if (dist < record)
                        {
                            record = dist;
                            closest_point = r_hit.point;
                        }
                    }
                }

                if (record != 1e8)
                {
                    fill(255);
                    line(m_position.x, m_position.y, closest_point.x, closest_point.y);
                }
            }
        }

        void Particle::show()
        {
            fill(255);
            circle(m_position.x, m_position.y, 1.0f);

            for (const Ray& r : m_rays)
            {
                r.show();
            }
        }
    }
}