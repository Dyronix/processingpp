#include "transform.h"
#include "trigonometry.h"
#include "render/render.h"
#include "util/transform_stack.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace ppp
{
    void push()
    {
        transform_stack::push();
    }

    void pop()
    {
        transform_stack::pop();
    }

    mat4 active_transform()
    {
        return transform_stack::active_world();
    }

    void rotate(float angle)
    {
        switch (angle_mode())
        {
        case angle_mode_type::DEGREES: transform_stack::rotate(radians(angle)); break;
        case angle_mode_type::RADIANS: transform_stack::rotate(angle); break;
        }
    }

    void rotate(float axisx, float axisy, float axisz, float angle)
    {
        switch (angle_mode())
        {
        case angle_mode_type::DEGREES: transform_stack::rotate({ axisx, axisy, axisz }, radians(angle)); break;
        case angle_mode_type::RADIANS: transform_stack::rotate({ axisx, axisy, axisz }, angle); break;
        }
    }

    void rotate(const vec3& axis, float angle)
    {
        rotate(axis.x, axis.y, axis.y, angle);
    }

    void scale(float x, float y)
    {
        transform_stack::scale(glm::vec2(x, y));
    }

    void scale(float x, float y, float z)
    {
        transform_stack::scale(glm::vec3(x, y, z));
    }

    void scale(const vec2& s)
    {
        scale(s.x, s.y);
    }

    void scale(const vec3& s)
    {
        scale(s.x, s.y, s.z);
    }

    void translate(float x, float y)
    {
        transform_stack::translate(glm::vec2(x, y));
    }

    void translate(float x, float y, float z)
    {
        transform_stack::translate(glm::vec3(x, y, z));
    }

    void translate(const vec2& t)
    {
        translate(t.x, t.y);
    }

    void translate(const vec3& t)
    {
        translate(t.x, t.y, t.z);
    }

    void transform(const vec3& position, const quat& rotation, const vec3& scale)
    {
        if (position.length() != 0.0f)
        {
            ppp::translate(position);
        }

        auto axis = glm::axis(rotation);
        auto angle = glm::angle(rotation);
        if (angle != 0.0f && axis.length() != 0.0f)
        {
           ppp::rotate(axis, angle);
        }

        if (scale.length() != 0.0f)
        {
            ppp::scale(scale);
        }
    }
}