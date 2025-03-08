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
                case light_type::AMBIENT: return lights_pool::light_type::AMBIENT;
                case light_type::DIRECTIONAL: return lights_pool::light_type::DIRECTIONAL;
                case light_type::POINT: return lights_pool::light_type::POINT;
                default:
                    log::error("Trying to parse unknown type, exiting program");
                    exit(EXIT_FAILURE);
                }

                return {};
            }
        }
        
        //-------------------------------------------------------------------------
        void no_lights()
        {
            lights_pool::clear();
        }
        
        //-------------------------------------------------------------------------
        light_id ambient_light(const ambient_light_desc& desc)
        {
            return lights_pool::add_ambient_light({desc.r, desc.g, desc.b}, desc.intensity);
        }
        //-------------------------------------------------------------------------
        light_id directional_light(const directional_light_desc& desc)
        {
            return lights_pool::add_directional_light({ desc.dirx, desc.diry, desc.dirz }, { desc.r, desc.g, desc.b }, desc.intensity, { desc.spec_r, desc.spec_g, desc.spec_b }, desc.cast_shadows);
        }
        //-------------------------------------------------------------------------
        light_id point_light(const point_light_desc& desc)
        {
            return lights_pool::add_point_light({ desc.x, desc.y, desc.z}, { desc.r, desc.g, desc.b }, desc.intensity, { desc.spec_r, desc.spec_g, desc.spec_b }, desc.cast_shadows, desc.range, desc.falloff);
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
        void light_intensity(light_id id, light_type type, float intensity)
        {
            lights_pool::light_intensity(id, conversions::to_light_type(type), intensity);
        }
        //-------------------------------------------------------------------------
        void light_enable_shadows(light_id id, light_type type, bool enable)
        {
            lights_pool::light_enable_shadows(id, conversions::to_light_type(type), enable);
        }
        //-------------------------------------------------------------------------
        void light_range(light_id id, light_type type, float range)
        {
            lights_pool::light_range(id, conversions::to_light_type(type), range);
        }
        //-------------------------------------------------------------------------
        void light_falloff(light_id id, light_type type, float falloff)
        {
            lights_pool::light_falloff(id, conversions::to_light_type(type), falloff);
        }
        //-------------------------------------------------------------------------
        void light_specular_color(light_id id, light_type type, float r, float g, float b)
        {
            lights_pool::light_specular_color(id, conversions::to_light_type(type), {r, g, b});
        }
    }
}