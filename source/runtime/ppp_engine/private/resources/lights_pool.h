#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace lights_pool
    {
        using light_id = s32;

        enum class light_type
        {
            POINT,
            DIRECTIONAL
        };

        struct point_light
        {
            light_id    id                  = -1;
            glm::vec3   position            = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3   ambient             = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3   diffuse             = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3   specular            = glm::vec3(1.0f, 1.0f, 1.0f);
            bool        specular_enabled    = false;
            bool        cast_shadows        = false;
            f32         constant            = 1.0f;
            f32         linear              = 0.09f;
            f32         quadratic           = 0.032f;
        };

        struct directional_light
        {
            light_id    id                  = -1;
            glm::vec3   direction           = glm::vec3(1.0f, 1.0f, 0.0f);
            glm::vec3   ambient             = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3   diffuse             = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3   specular            = glm::vec3(1.0f, 1.0f, 1.0f);
            bool        specular_enabled    = false;
            bool        cast_shadows        = false;
        };

        void clear();

        light_id add_directional_light(const glm::vec3& direction, const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f), bool specular_enabled = false, bool cast_shadows = false);
        light_id add_point_light(const glm::vec3& position, const glm::vec3& ambient = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& specular = glm::vec3(1.0f, 1.0f, 1.0f), bool specular_enabled = false, bool cast_shadows = false, f32 constant = 1.0f, f32 linear = 0.09f, f32 quadratic = 0.032f);

        void light_position(light_id id, light_type type, const glm::vec3& position);
        void light_direction(light_id id, light_type type, const glm::vec3& direction);
        void light_ambient(light_id id, light_type type, const glm::vec3& ambient_color);
        void light_diffuse(light_id id, light_type type, const glm::vec3& diffuse_color);
        void light_specular(light_id id, light_type type, const glm::vec3& specular_color);
        void light_enable_specular(light_id id, light_type type, bool enable);
        void light_enable_shadows(light_id id, light_type type, bool enable);

        const graphics_vector<point_light>& point_lights();
        const graphics_vector<directional_light>& directional_lights();
    }
}