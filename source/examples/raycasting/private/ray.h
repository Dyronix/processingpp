#pragma once

#include "vector.h"

namespace ppp
{
    namespace raycasting
    {
        struct RaycastHit
        {
            float t;
            vec3 point;
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
            vec2 m_position;
            vec2 m_direction;

            float m_length;
        };
    }
}