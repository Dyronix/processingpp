#include "util/transform_stack.h"

#include "memory/memory_types.h"

#include <glm/ext/matrix_transform.hpp>

namespace ppp
{
    namespace transform_stack
    {
        namespace internal
        {
            static temp_vector<glm::mat4>& model_matrices()
            {
                static temp_vector<glm::mat4> s_model_matrices;

                return s_model_matrices;
            }
        }

        void push()
        {
            internal::model_matrices().push_back(glm::identity<glm::mat4>());
        }

        void pop()
        {
            internal::model_matrices().pop_back();
        }

        void rotate(f32 angle)
        {
            internal::model_matrices().back() = glm::rotate(internal::model_matrices().back(), angle, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        void scale(const glm::vec2& scale)
        {
            internal::model_matrices().back() = glm::scale(internal::model_matrices().back(), glm::vec3(scale, 1.0f));
        }

        void scale(const glm::vec3& scale)
        {
            internal::model_matrices().back() = glm::scale(internal::model_matrices().back(), scale);
        }

        void translate(const glm::vec2& translate)
        {
            internal::model_matrices().back() = glm::translate(internal::model_matrices().back(), glm::vec3(translate, 1.0f));
        }

        void translate(const glm::vec3& translate)
        {
            internal::model_matrices().back() = glm::translate(internal::model_matrices().back(), translate);
        }

        glm::mat4 active_world()
        {
            if (internal::model_matrices().empty())
            {
                return glm::identity<glm::mat4>();
            }

            glm::mat4 world = glm::identity<glm::mat4>();

            for (const glm::mat4& m : internal::model_matrices())
            {
                world *= m;
            }

            return world;
        }
    }
}