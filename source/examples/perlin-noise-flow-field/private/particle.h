#pragma once

#include <deque>
#include <vector>

#include "vector.h"

namespace ppp
{
    namespace flow_field
    {
        class Particle
        {
        public:
            Particle(float x, float y, float max_speed, int max_previous_positions);

            void show() const;
            void follow(const std::vector<vec2>& flow_field, int flow_field_rows, int flow_field_cols, float scale);
            void update();
            void apply_force(const vec2& force);
            void loop_around_edges();

            const vec2& position() const { return m_position; }
            const vec2& velocity() const { return m_velocity; }
            const vec2& acceleration() const { return m_acceleration; }

        private:
            vec2 m_position;
            vec2 m_velocity;
            vec2 m_acceleration;

            float m_max_speed;

            int m_max_previous_positions;
            std::deque<vec2> m_previous_positions;
        };
    }
}