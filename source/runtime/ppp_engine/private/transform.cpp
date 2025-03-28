#include "transform.h"
#include "trigonometry.h"
#include "render/render.h"
#include "util/transform_stack.h"

namespace ppp
{
    namespace transform
    {
        void push()
        {
            transform_stack::push();
        }

        void pop()
        {
            transform_stack::pop();
        }

        void rotate(float angle)
        {
            switch (trigonometry::angle_mode())
            {
            case trigonometry::angle_mode_type::DEGREES: transform_stack::rotate(trigonometry::radians(angle)); break;
            case trigonometry::angle_mode_type::RADIANS: transform_stack::rotate(angle); break;
            }
        }

        void rotate(float axisx, float axisy, float axisz, float angle)
        {
            switch (trigonometry::angle_mode())
            {
            case trigonometry::angle_mode_type::DEGREES: transform_stack::rotate({ axisx, axisy, axisz }, trigonometry::radians(angle)); break;
            case trigonometry::angle_mode_type::RADIANS: transform_stack::rotate({ axisx, axisy, axisz }, angle); break;
            }
        }
        
        void scale(float x, float y)
        {
            transform_stack::scale(glm::vec2(x, y));
        }

        void scale(float x, float y, float z)
        {
            transform_stack::scale(glm::vec3(x, y, z));
        }

        void translate(float x, float y)
        {
            transform_stack::translate(glm::vec2(x, y));
        }

        void translate(float x, float y, float z)
        {
            transform_stack::translate(glm::vec3(x, y, z));
        }
    }
}