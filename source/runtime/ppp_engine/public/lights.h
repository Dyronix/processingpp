#pragma once

#include "vector.h"
#include "color.h"

namespace ppp
{
    using light_id = int;

    enum class light_type
    {
        POINT,
        DIRECTIONAL
    };

    struct point_light_desc
    {
        vec3    position;
        color   ambient;
        color   diffuse;
        color   specular;
        bool    specular_enabled = false;
        bool    cast_shadows = false;
        float   max_range = 400.0f;
        float   falloff_start = 250.0f;
    };

    struct directional_light_desc
    {
        vec3    direction;
        color   ambient;
        color   diffuse;
        color   specular;
        bool    specular_enabled = false;
        bool    cast_shadows = false;
    };

    void no_lights();

    light_id directional_light(const directional_light_desc& desc);
    light_id point_light(const point_light_desc& desc);

    void light_position(light_id id, light_type type, float x, float y, float z);
    void light_direction(light_id id, light_type type, float x, float y, float z);
    void light_ambient(light_id id, light_type type, float r, float g, float b);
    void light_diffuse(light_id id, light_type type, float r, float g, float b);
    void light_specular(light_id id, light_type type, float r, float g, float b);
    void light_enable_specular(light_id id, light_type type, bool enable);
    void light_enable_shadows(light_id id, light_type type, bool enable);
}