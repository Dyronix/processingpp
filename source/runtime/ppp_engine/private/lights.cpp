#include "lights.h"

#include "resources/lights_pool.h"

#include "util/log.h"

namespace ppp
{
    namespace lights
    {
        namespace conversions
        {
            lights_pool::light_type to_light_type(lights::light_type type)
            {
                switch (type)
                {
                case light_type::DIRECTIONAL: return lights_pool::light_type::DIRECTIONAL;
                case light_type::POINT: return lights_pool::light_type::POINT;
                default:
                    log::error("Trying to parse unknown type, exiting program");
                    exit(EXIT_FAILURE);
                }

                return {};
            }
        }

        struct attenuation_coefficients
        {
            f32 constant;
            f32 linear;
            f32 quadratic;
        };

        //-------------------------------------------------------------------------
        attenuation_coefficients calculate_attenuation_coefficients(f32 max_range, f32 falloff_start)
        {
            attenuation_coefficients coeff;

            // Full intensity until falloff start:
            coeff.constant = 1.0f;

            // The effective range over which falloff happens:
            f32 range = max_range - falloff_start;

            // Desired intensity at max range:
            f32 cutoff = 0.05f;

            // We want: 1/(1 + linear*R + quadratic*R*R) = cutoff,
            // and we assume linear = quadratic * R.
            // Then 1 + 2 * quadratic * R² = 1 / cutoff.
            coeff.quadratic = ((1.0f / cutoff) - 1.0f) / (2.0f * range * range);
            coeff.linear = coeff.quadratic * range;

            return coeff;
        }
        
        //-------------------------------------------------------------------------
        void no_lights()
        {
            lights_pool::clear();
        }
        
        //-------------------------------------------------------------------------
        light_id directional_light(const directional_light_desc& desc)
        {
            glm::vec3 dir(desc.dir_x, desc.dir_y, desc.dir_z);
            glm::vec3 ambient(desc.ambient_r, desc.ambient_g, desc.ambient_b);
            glm::vec3 diffuse(desc.diffuse_r, desc.diffuse_g, desc.diffuse_b);
            glm::vec3 specular(desc.specular_r, desc.specular_g, desc.specular_b);
            bool specular_enabled = desc.specular_enabled;
            bool cast_shadows = desc.cast_shadows;
            return lights_pool::add_directional_light(dir, ambient, diffuse, specular, specular_enabled, cast_shadows);
        }
        //-------------------------------------------------------------------------
        light_id point_light(const point_light_desc& desc)
        {
            glm::vec3 pos(desc.x, desc.y, desc.z);
            glm::vec3 ambient(desc.ambient_r, desc.ambient_g, desc.ambient_b);
            glm::vec3 diffuse(desc.diffuse_r, desc.diffuse_g, desc.diffuse_b);
            glm::vec3 specular(desc.specular_r, desc.specular_g, desc.specular_b);
            bool specular_enabled = desc.specular_enabled;
            bool cast_shadows = desc.cast_shadows;
            attenuation_coefficients coeff = calculate_attenuation_coefficients(desc.max_range, desc.falloff_start);
            return lights_pool::add_point_light(pos, ambient, diffuse, specular, specular_enabled, cast_shadows, coeff.constant, coeff.linear, coeff.quadratic);
        }

        //-------------------------------------------------------------------------
        void light_position(light_id id, light_type type, float x, float y, float z)
        {
            lights_pool::light_position(id, conversions::to_light_type(type), {x, y, z});
        }
        //-------------------------------------------------------------------------
        void light_direction(light_id id, light_type type, float x, float y, float z)
        {
            lights_pool::light_direction(id, conversions::to_light_type(type), {x, y, z});
        }
        //-------------------------------------------------------------------------
        void light_ambient(light_id id, light_type type, float r, float g, float b)
        {
            lights_pool::light_ambient(id, conversions::to_light_type(type), { r, g, b });
        }
        //-------------------------------------------------------------------------
        void light_diffuse(light_id id, light_type type, float r, float g, float b)
        {
            lights_pool::light_diffuse(id, conversions::to_light_type(type), { r, g, b });
        }
        //-------------------------------------------------------------------------
        void light_specular(light_id id, light_type type, float r, float g, float b)
        {
            lights_pool::light_specular(id, conversions::to_light_type(type), { r, g, b });
        }
        //-------------------------------------------------------------------------
        void light_enable_specular(light_id id, light_type type, bool enable)
        {
            lights_pool::light_enable_specular(id, conversions::to_light_type(type), enable);
        }
        //-------------------------------------------------------------------------
        void light_enable_shadows(light_id id, light_type type, bool enable)
        {
            lights_pool::light_enable_shadows(id, conversions::to_light_type(type), enable);
        }
    }
}