#include "particle.h"
#include "shapes.h"
#include "color.h"
#include "environment.h"
#include "trigonometry.h"
#include "mathematics.h"

#include <algorithm>

namespace ppp
{
    namespace flow_field
    {   
        Particle::Particle(float x, float y, float max_speed, int max_previous_positions)
            : m_position(x, y)
            , m_velocity(0.0f, 0.0f)
            , m_acceleration(0.0f, 0.0f)
            , m_max_speed(max_speed)
            , m_max_previous_positions(max_previous_positions)
        {

        }

        void Particle::show() const
        {
            int index = 0;
            std::for_each(std::rbegin(m_previous_positions), std::rend(m_previous_positions),
                [&](const vec2& p)
            {
                float t = index / static_cast<float>(m_previous_positions.size());

                fill(255, 0, 0, static_cast<int>(math::lerp(0.0f, 255.0f, t)));
                point(p.x, p.y);
                ++index;
            });

            fill(255, 0, 0, 255);
            point(m_position.x, m_position.y);
        }

        void Particle::follow(const std::vector<vec2>& flow_field, int flow_field_rows, int flow_field_cols, float scale)
        {
            float x = std::floor(m_position.x / scale);
            float y = std::floor(m_position.y / scale);

            int index = x + (y * flow_field_cols);

            apply_force(flow_field[index]);
        }

        void Particle::update()
        {
            if (m_previous_positions.size() > m_max_previous_positions)
            {
                m_previous_positions.pop_back();
            }

            m_previous_positions.push_front(m_position);

            m_velocity += m_acceleration;
            m_velocity.limit(m_max_speed);
            m_position += m_velocity;

            m_acceleration = m_acceleration * 0.0f;
        }

        void Particle::apply_force(const vec2& force)
        {
            m_acceleration += force;
        }

        void Particle::loop_around_edges()
        {
            if (m_position.x >= canvas_width()) m_position.x = 0;
            if (m_position.x < 0) m_position.x = canvas_width() - 1;
            if (m_position.y >= canvas_height()) m_position.y = 0;
            if (m_position.y < 0) m_position.y = canvas_height() - 1;
        }
    }
}