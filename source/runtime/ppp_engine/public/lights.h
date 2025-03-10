#pragma once

namespace ppp
{
    namespace lights
    {
        using light_id = int;

        enum class light_type
        {
            POINT,
            DIRECTIONAL
        };

        struct point_light_desc
        {
            float   x, y, z;
            float   ambient_r, ambient_g, ambient_b = 1.0f;
            float   diffuse_r, diffuse_g, diffuse_b = 1.0f;
            float   specular_r, specular_g, specular_b = 1.0f;
            bool    specular_enabled = false;
            bool    cast_shadows = false;
            float   max_range = 400.0f;
            float   falloff_start = 250.0f;
        };

        struct directional_light_desc
        {
            float   dir_x, dir_y, dir_z;
            float   ambient_r, ambient_g, ambient_b = 1.0f;
            float   diffuse_r, diffuse_g, diffuse_b = 1.0f;
            float   specular_r, specular_g, specular_b = 1.0f;
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
}