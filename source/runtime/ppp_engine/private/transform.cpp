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
            transform::push();
        }

        void pop()
        {
            transform::pop();
        }

        void rotate(float angle)
        {
            switch (trigonometry::angle_mode())
            {
            case trigonometry::AngleMode::DEGREES: transform::rotate(trigonometry::radians(angle)); break;
            case trigonometry::AngleMode::RADIANS: transform::rotate(angle); break;
            }
        }
        
        void scale(float x, float y)
        {
            transform::scale(glm::vec2(x, y));
        }

        void scale(float x, float y, float z)
        {
            transform::scale(glm::vec3(x, y, z));
        }

        void translate(float x, float y)
        {
            transform::translate(glm::vec2(x, y));
        }

        void translate(float x, float y, float z)
        {
            transform::translate(glm::vec3(x, y, z));
        }
    }
}