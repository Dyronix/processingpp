#include "render/render_transform.h"

#include <glm/ext/matrix_transform.hpp>

namespace ppp
{
    namespace render
    {
        namespace transform
        {
            namespace internal
            {
                std::vector<glm::mat4> _model_matrices;
            }

            void push()
            {
                internal::_model_matrices.push_back(glm::identity<glm::mat4>());
            }

            void pop()
            {
                internal::_model_matrices.pop_back();
            }

            void rotate(f32 angle)
            {
                internal::_model_matrices.back() = glm::rotate(internal::_model_matrices.back(), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
            }

            void scale(const glm::vec2& scale)
            {
                internal::_model_matrices.back() = glm::scale(internal::_model_matrices.back(), glm::vec3(scale, 1.0f));
            }

            void translate(const glm::vec2& translate)
            {
                internal::_model_matrices.back() = glm::translate(internal::_model_matrices.back(), glm::vec3(translate, 1.0f));
            }

            glm::mat4 active_world()
            {
                if (internal::_model_matrices.empty())
                {
                    return glm::identity<glm::mat4>();
                }

                glm::mat4 world = glm::identity<glm::mat4>();

                for (const glm::mat4& m : internal::_model_matrices)
                {
                    world *= m;
                }

                return world;
            }
        }
    }
}