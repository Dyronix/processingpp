#include "transform.h"
#include "trigonometry.h"
#include "render/render.h"
#include "render/render_transform.h"

namespace ppp
{
    namespace transform
    {
        void push()
        {
            render::transform::push();
        }

        void pop()
        {
            render::transform::pop();
        }

        void rotate(float angle)
        {
            switch (trigonometry::angle_mode())
            {
            case trigonometry::AngleMode::DEGREES: render::transform::rotate(trigonometry::radians(angle)); break;
            case trigonometry::AngleMode::RADIANS: render::transform::rotate(angle); break;
            }
        }
        
        void scale(float x, float y)
        {
            render::transform::scale(glm::vec2(x, y));
        }

        void scale(float x, float y, float z)
        {
            render::transform::scale(glm::vec3(x, y, z));
        }

        void translate(float x, float y)
        {
            render::transform::translate(glm::vec2(x, y));
        }

        void translate(float x, float y, float z)
        {
            render::transform::translate(glm::vec3(x, y, z));
        }
    }
}