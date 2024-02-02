#pragma once

#include <glm/glm.hpp>

namespace ppp
{
    namespace raycasting
    {
        struct RaycastHit
        {
            float t;
            glm::vec3 point;
            bool hit;

            operator bool() const
            {
                return hit;
            }
        };

        class Ray
        {
        public:
            Ray(float posx, float posy, float angle);

            void lookat(float x, float y);

            void set_direction(float x, float y);
            void set_position(float x, float y);

            void show() const;
            
            RaycastHit cast(float x1, float y1, float x2, float y2) const;

        private:
            glm::vec2 m_position;
            glm::vec2 m_direction;

            float m_length;
        };
    }
}