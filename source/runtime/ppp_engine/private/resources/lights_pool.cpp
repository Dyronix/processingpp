#include "resources/lights_pool.h"

#include "util/log.h"

#include "string/string_id.h"

namespace ppp
{
    namespace lights_pool
    {
        namespace conversions
        {
            //-------------------------------------------------------------------------
            string::string_id to_string(light_type type)
            {
                switch (type)
                {
                case light_type::AMBIENT: return string::store_sid("Ambient");
                case light_type::POINT: return string::store_sid("Point");
                case light_type::DIRECTIONAL: return string::store_sid("Directional");
                    break;
                default:
                    log::error("Trying to parse unknown type, exiting program");
                }

                return {};
            }
        }

        struct context
        {
            graphics_vector<ambient_light>      ambient_lights;
            graphics_vector<point_light>        point_lights;
            graphics_vector<directional_light>  directional_lights;

            //-------------------------------------------------------------------------
            ambient_light* find_ambient_light(light_id id)
            {
                auto it = std::find_if(std::begin(g_ctx.ambient_lights), std::end(g_ctx.ambient_lights),
                    [id](const ambient_light& light)
                {
                    return id == light.id;
                });

                if (it == std::cend(ambient_lights))
                {
                    return nullptr;
                }

                return &(*it);
            }

            //-------------------------------------------------------------------------
            point_light* find_point_light(light_id id)
            {
                auto it = std::find_if(std::begin(g_ctx.point_lights), std::end(g_ctx.point_lights),
                    [id](const point_light& light)
                {
                    return id == light.id;
                });

                if (it == std::cend(point_lights))
                {
                    return nullptr;
                }

                return &(*it);
            }

            //-------------------------------------------------------------------------
            directional_light* find_directional_light(light_id id)
            {
                auto it = std::find_if(std::begin(g_ctx.directional_lights), std::end(g_ctx.directional_lights),
                    [id](const directional_light& light)
                {
                    return id == light.id;
                });

                if (it == std::end(directional_lights))
                {
                    return nullptr;
                }

                return &(*it);
            }
        } g_ctx;

        //-------------------------------------------------------------------------
        void clear()
        {
            g_ctx.ambient_lights.clear();
            g_ctx.point_lights.clear();
            g_ctx.directional_lights.clear();
        }

        //-------------------------------------------------------------------------
        light_id add_ambient_light(const glm::vec3& color, f32 intensity)
        {
            light_id light_id = g_ctx.ambient_lights.size() + g_ctx.point_lights.size() + g_ctx.directional_lights.size();

            g_ctx.ambient_lights.push_back(ambient_light{ light_id, intensity, color });

            return light_id;
        }
        //-------------------------------------------------------------------------
        light_id add_directional_light(const glm::vec3& direction, const glm::vec3& color, f32 intensity, const glm::vec3& specular_color, bool specular_enabled, bool cast_shadows)
        {
            light_id light_id = g_ctx.ambient_lights.size() + g_ctx.point_lights.size() + g_ctx.directional_lights.size();

            g_ctx.directional_lights.push_back(directional_light{ light_id, intensity, direction, color, specular_color, specular_enabled, cast_shadows });

            return light_id;
        }
        //-------------------------------------------------------------------------
        light_id add_point_light(const glm::vec3& position, const glm::vec3& color, f32 intensity, const glm::vec3& specular_color, bool specular_enabled, bool cast_shadows, f32 range, f32 falloff, f32 threshold)
        {
            light_id light_id = g_ctx.ambient_lights.size() + g_ctx.point_lights.size() + g_ctx.directional_lights.size();

            g_ctx.point_lights.push_back(point_light{ light_id, intensity, position, color, specular_color, specular_enabled, cast_shadows, range, falloff, threshold });

            return light_id;
        }

        //-------------------------------------------------------------------------
        void light_position(light_id id, light_type type, const glm::vec3& position)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point) 
                    point->position = position;
                break;
            }

            default:
                log::warn("Unable to change range for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_direction(light_id id, light_type type, const glm::vec3& direction)
        {
            switch (type)
            {
            case light_type::DIRECTIONAL:
            {
                auto dir = g_ctx.find_directional_light(id);
                if (dir) 
                    dir->direction = direction;
                break;
            }

            default:
                log::warn("Unable to change range for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_intensity(light_id id, light_type type, f32 intensity)
        {
            switch (type)
            {
            case light_type::AMBIENT:
            {
                auto ambient = g_ctx.find_ambient_light(id);
                if (ambient) 
                    ambient->intensity = intensity;
                break;
            }
            case light_type::DIRECTIONAL:
            {
                auto dir = g_ctx.find_directional_light(id);
                if (dir) 
                    dir->intensity = intensity;
                break;
            }
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point) 
                    point->intensity = intensity;
                break;
            }

            default:
                log::warn("Unable to change intensity for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_enable_shadows(light_id id, light_type type, bool enable)
        {
            switch (type)
            {
                case light_type::POINT:
                {
                    auto point = g_ctx.find_point_light(id);
                    if (point)
                        point->cast_shadows = enable;
                    break;
                }
                case light_type::DIRECTIONAL:
                {
                    auto dir = g_ctx.find_directional_light(id);
                    if (dir) 
                        dir->cast_shadows = enable;
                    break;
                }

                default:
                    log::warn("Unable to enable/disable shadows for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_range(light_id id, light_type type, f32 range)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point)
                    point->range = range;
                break;
            }

            default:
                log::warn("Unable to change range for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_falloff(light_id id, light_type type, f32 falloff)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point) 
                    point->falloff = falloff;
                break;
            }

            default:
                log::warn("Unable to change falloff for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_specular_color(light_id id, light_type type, const glm::vec3& specular_color)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point)
                    point->specular_color = specular_color;
                break;
            }
            case light_type::DIRECTIONAL:
            {
                auto dir = g_ctx.find_directional_light(id);
                if (dir) 
                    dir->specular_color = specular_color;
                break;
            }

            default:
                log::warn("Unable to change specular color for light of type: {}", conversions::to_string(type).str());
            }
        }

        //-------------------------------------------------------------------------
        const graphics_vector<ambient_light>& ambient_lights()
        {
            return g_ctx.ambient_lights;
        }
        //-------------------------------------------------------------------------
        const graphics_vector<point_light>& point_lights()
        {
            return g_ctx.point_lights;
        }
        //-------------------------------------------------------------------------
        const graphics_vector<directional_light>& directional_lights()
        {
            return g_ctx.directional_lights;
        }
    }
}