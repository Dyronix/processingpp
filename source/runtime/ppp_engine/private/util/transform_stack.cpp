#include "util/transform_stack.h"

#include "memory/memory_types.h" 
#include "memory/memory_placement_new.h" 
#include "memory/memory_manager.h"

#include <glm/ext/matrix_transform.hpp>

namespace ppp
{
    namespace transform_stack
    {
        using model_matrices_arr = temp_vector<glm::mat4>;

        static model_matrices_arr& model_matrices()
        {
            static model_matrices_arr s_model_matrices;

            return s_model_matrices;
        }

        void push()
        {
            auto& matrices = model_matrices();

            matrices.push_back(glm::identity<glm::mat4>());
        }

        void pop()
        {
            auto& matrices = model_matrices();

            matrices.pop_back();
        }

        void rotate(f32 angle)
        {
            model_matrices().back() = glm::rotate(model_matrices().back(), angle, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        void rotate(const glm::vec3& axis, f32 angle)
        {
            model_matrices().back() = glm::rotate(model_matrices().back(), angle, axis);
        }

        void scale(const glm::vec2& scale)
        {
            model_matrices().back() = glm::scale(model_matrices().back(), glm::vec3(scale, 1.0f));
        }

        void scale(const glm::vec3& scale)
        {
            model_matrices().back() = glm::scale(model_matrices().back(), scale);
        }

        void translate(const glm::vec2& translate)
        {
            model_matrices().back() = glm::translate(model_matrices().back(), glm::vec3(translate, 1.0f));
        }

        void translate(const glm::vec3& translate)
        {
            model_matrices().back() = glm::translate(model_matrices().back(), translate);
        }

        glm::mat4 active_world()
        {
            if (model_matrices().empty())
            {
                return glm::identity<glm::mat4>();
            }

            glm::mat4 world = glm::identity<glm::mat4>();

            for (const glm::mat4& m : model_matrices())
            {
                world *= m;
            }

            return world;
        }
    }
}