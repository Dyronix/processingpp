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
            bool                                point_lights_with_shadow_enabled = false;
            bool                                directional_lights_with_shadow_enabled = false;

            std::vector<point_light>        point_lights;
            std::vector<directional_light>  directional_lights;

            //-------------------------------------------------------------------------
            s32 count_point_lights_shadow_enabled() const
            {
                s32 num = std::count_if(std::cbegin(point_lights), std::cend(point_lights),
                    [](const point_light& light)
                    {
                        return light.cast_shadows;
                    });

                return num;
            }

            //-------------------------------------------------------------------------
            s32 count_directional_lights_shadow_enabled() const
            {
                s32 num = std::count_if(std::cbegin(directional_lights), std::cend(directional_lights),
                    [](const directional_light& light)
                    {
                        return light.cast_shadows;
                    });

                return num;
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
            g_ctx.point_lights.clear();
            g_ctx.directional_lights.clear();
        }

        //-------------------------------------------------------------------------
        light_id add_directional_light(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool specular_enabled, bool cast_shadows)
        {
            light_id light_id = g_ctx.point_lights.size() + g_ctx.directional_lights.size();

            g_ctx.directional_lights.push_back(directional_light{light_id, direction, ambient, diffuse, specular, specular_enabled, cast_shadows});
            g_ctx.directional_lights_with_shadow_enabled = g_ctx.count_directional_lights_shadow_enabled() > 0;

            return light_id;
        }
        //-------------------------------------------------------------------------
        light_id add_point_light(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool specular_enabled, bool cast_shadows, f32 constant, f32 linear, f32 quadratic)
        {
            light_id light_id = g_ctx.point_lights.size() + g_ctx.directional_lights.size();

            g_ctx.point_lights.push_back(point_light{light_id, position, ambient, diffuse, specular, specular_enabled, cast_shadows, constant, linear, quadratic});
            g_ctx.point_lights_with_shadow_enabled = g_ctx.count_point_lights_shadow_enabled() > 0;

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
        void light_enable_specular(light_id id, light_type type, bool enable)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point)
                    point->specular_enabled = enable;
                break;
            }
            case light_type::DIRECTIONAL:
            {
                auto dir = g_ctx.find_directional_light(id);
                if (dir)
                    dir->specular_enabled = enable;
                break;
            }

            default:
                log::warn("Unable to enable/disable specular for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_ambient(light_id id, light_type type, const glm::vec3& ambient_color)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point)
                    point->ambient = ambient_color;
                break;
            }
            case light_type::DIRECTIONAL:
            {
                auto dir = g_ctx.find_directional_light(id);
                if (dir)
                    dir->ambient = ambient_color;
                break;
            }

            default:
                log::warn("Unable to change specular color for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_diffuse(light_id id, light_type type, const glm::vec3& diffuse_color)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point)
                    point->diffuse = diffuse_color;
                break;
            }
            case light_type::DIRECTIONAL:
            {
                auto dir = g_ctx.find_directional_light(id);
                if (dir)
                    dir->diffuse = diffuse_color;
                break;
            }

            default:
                log::warn("Unable to change specular color for light of type: {}", conversions::to_string(type).str());
            }
        }
        //-------------------------------------------------------------------------
        void light_specular(light_id id, light_type type, const glm::vec3& specular_color)
        {
            switch (type)
            {
            case light_type::POINT:
            {
                auto point = g_ctx.find_point_light(id);
                if (point)
                    point->specular = specular_color;
                break;
            }
            case light_type::DIRECTIONAL:
            {
                auto dir = g_ctx.find_directional_light(id);
                if (dir)
                    dir->specular = specular_color;
                break;
            }

            default:
                log::warn("Unable to change specular color for light of type: {}", conversions::to_string(type).str());
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
                    {
                        point->cast_shadows = enable;
                        g_ctx.point_lights_with_shadow_enabled = g_ctx.count_point_lights_shadow_enabled() > 0;
                    }
                    break;
                }
                case light_type::DIRECTIONAL:
                {
                    auto dir = g_ctx.find_directional_light(id);
                    if (dir)
                    {
                        dir->cast_shadows = enable;
                        g_ctx.directional_lights_with_shadow_enabled = g_ctx.count_directional_lights_shadow_enabled() > 0;
                    }
                    break;
                }

                default:
                    log::warn("Unable to enable/disable shadows for light of type: {}", conversions::to_string(type).str());
            }
        }

        //-------------------------------------------------------------------------
        const std::vector<point_light>& point_lights()
        {
            return g_ctx.point_lights;
        }
        //-------------------------------------------------------------------------
        const std::vector<directional_light>& directional_lights()
        {
            return g_ctx.directional_lights;
        }

        //-------------------------------------------------------------------------
        bool has_point_lights_with_shadow()
        {
            return g_ctx.point_lights_with_shadow_enabled;
        }

        //-------------------------------------------------------------------------
        bool has_directional_lights_with_shadow()
        {
            return g_ctx.directional_lights_with_shadow_enabled;
        }
    }
}