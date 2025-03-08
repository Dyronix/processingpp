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
            AMBIENT,
            POINT,
            DIRECTIONAL
        };

        struct ambient_light
        {
            light_id    id                  = -1;
            f32         intensity           = 1.0f; 
            glm::vec3   color               = glm::vec3(1.0f, 1.0f, 1.0f);
        };

        struct point_light
        {
            light_id    id                  = -1;
            f32         intensity           = 1.0f;
            glm::vec3   position            = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3   color               = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3   specular_color      = glm::vec3(1.0f, 1.0f, 1.0f);
            bool        specular_enabled    = false;
            bool        cast_shadows        = false;
            f32         range               = 100.0f;
            f32         falloff             = 50.0f;
            f32         threshold           = 0.02f;
        };

        struct directional_light
        {
            light_id    id                  = -1;
            f32         intensity           = 1.0f;
            glm::vec3   direction           = glm::vec3(1.0f, 1.0f, 0.0f);
            glm::vec3   color               = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3   specular_color      = glm::vec3(1.0f, 1.0f, 1.0f);
            bool        specular_enabled    = false;
            bool        cast_shadows        = false;
        };

        void clear();

        light_id add_ambient_light(const glm::vec3& color, f32 intensity = 1.0f);
        light_id add_directional_light(const glm::vec3& direction, const glm::vec3& color, f32 intensity = 1.0f, const glm::vec3& specular_color = glm::vec3(1.0f, 1.0f, 1.0f), bool specular_enabled = false, bool cast_shadows = false);
        light_id add_point_light(const glm::vec3& position, const glm::vec3& color, f32 intensity = 1.0f, const glm::vec3& specular_color = glm::vec3(1.0f, 1.0f, 1.0f), bool specular_enabled = false, bool cast_shadows = false, f32 range = 100.0f, f32 falloff = 50.0f, f32 threshold = 0.02f);

        void light_position(light_id id, light_type type, const glm::vec3& position);
        void light_direction(light_id id, light_type type, const glm::vec3& direction);
        void light_intensity(light_id id, light_type, f32 intensity);
        void light_enable_shadows(light_id id, light_type type, bool enable);
        void light_range(light_id id, light_type, f32 range);
        void light_falloff(light_id id, light_type type, f32 falloff);
        void light_specular_color(light_id id, light_type type, const glm::vec3& specular_color);

        const graphics_vector<ambient_light>& ambient_lights();
        const graphics_vector<point_light>& point_lights();
        const graphics_vector<directional_light>& directional_lights();
    }
}