#pragma once

#include <deque>
#include <vector>

#include <glm/glm.hpp>

namespace ppp
{
    namespace flow_field
    {
        class Particle
        {
        public:
            Particle(float x, float y, float max_speed, int max_previous_positions);

            void show() const;
            void follow(const std::vector<glm::vec2>& flow_field, int flow_field_rows, int flow_field_cols, float scale);
            void update();
            void apply_force(const glm::vec2& force);
            void loop_around_edges();

            const glm::vec2& position() const { return m_position; }
            const glm::vec2& velocity() const { return m_velocity; }
            const glm::vec2& acceleration() const { return m_acceleration; }

        private:
            glm::vec2 m_position;
            glm::vec2 m_velocity;
            glm::vec2 m_acceleration;

            float m_max_speed;

            int m_max_previous_positions;
            std::deque<glm::vec2> m_previous_positions;
        };
    }
}