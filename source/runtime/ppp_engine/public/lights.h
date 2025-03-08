#pragma once

namespace ppp
{
    namespace lights
    {
        using light_id = int;

        enum class light_type
        {
            AMBIENT,
            POINT,
            DIRECTIONAL
        };

        struct ambient_light_desc
        {
            float r, g, b;
            float intensity = 1.0f;
        };

        struct directional_light_desc
        {
            float   dirx, diry, dirz;
            float   r, g, b;
            float   intensity = 1.0f;
            float   spec_r, spec_g, spec_b = 1.0f;
            bool    spec_enabled = false;
            bool    cast_shadows = false;
        };

        struct point_light_desc
        {
            float   x, y, z;
            float   r, g, b;
            float   intensity = 1.0f;
            float   spec_r, spec_g, spec_b = 1.0f;
            bool    spec_enabled = false;
            bool    cast_shadows = false;
            float   range = 100.0f;
            float   falloff = 200.0f;
            float   threshold = 0.02f;
        };

        void no_lights();

        light_id ambient_light(const ambient_light_desc& desc);
        light_id directional_light(const directional_light_desc& desc);
        light_id point_light(const point_light_desc& desc);

        void light_position(light_id id, light_type type, float x, float y, float z);
        void light_direction(light_id id, light_type type, float x, float y, float z);
        void light_intensity(light_id id, light_type type, float intensity);
        void light_enable_shadows(light_id id, light_type type, bool enable);
        void light_range(light_id id, light_type, float range);
        void light_falloff(light_id id, light_type type, float falloff);
        void light_specular_color(light_id id, light_type type, float r, float g, float b);
    }
}