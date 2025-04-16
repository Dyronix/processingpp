#include "transform.h"
#include "trigonometry.h"
#include "render/render.h"
#include "util/transform_stack.h"

namespace ppp
{
    mat4 convert_glm_to_ppp(const glm::mat4& m)
    {
        mat4 result;

        // glm::mat4 is column-major, so each m[i] represents a column (of type glm::vec4)
        result.cols[0] = base_vec4<float>(m[0].x, m[0].y, m[0].z, m[0].w);
        result.cols[1] = base_vec4<float>(m[1].x, m[1].y, m[1].z, m[1].w);
        result.cols[2] = base_vec4<float>(m[2].x, m[2].y, m[2].z, m[2].w);
        result.cols[3] = base_vec4<float>(m[3].x, m[3].y, m[3].z, m[3].w);
        
        return result;
    }

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
        return convert_glm_to_ppp(transform_stack::active_world());
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
}