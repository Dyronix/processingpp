#include "transform.h"
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
            render::transform::rotate(angle);
        }
        
        void scale(float x, float y)
        {
            render::transform::scale(glm::vec2(x, y));
        }

        void translate(float x, float y)
        {
            render::transform::translate(glm::vec2(x, y));
        }
    }
}