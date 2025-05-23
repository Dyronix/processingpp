/**
 * @file lights.h
 * @brief Light management utilities for directional and point lights within the scene.
 */
#pragma once

#include "vector.h"
#include "color.h"

namespace ppp
{
    /// @brief Unique identifier for a light source.
    using light_id = int;

    /**
     * @brief Types of lights supported.
     */
    enum class light_type
    {
        POINT,       
        DIRECTIONAL
    };

    /**
     * @brief Parameters for creating a point light.
     */
    struct point_light_desc
    {
        /** @brief Position of the light in world space */
        vec3 position;                   
        /** @brief Ambient color component */
        color ambient;                    
        /** @brief Diffuse color component */
        color diffuse;                    
        /** @brief Specular color component */
        color specular;                   
        /** @brief Enable specular highlights */
        bool specular_enabled = false;   
        /** @brief Enable shadow casting */
        bool cast_shadows = false;       
        /** @brief Maximum effective range */
        float max_range = 400.0f;         
        /** @brief Distance where falloff begins */
        float   falloff_start = 250.0f;     
    };

    /**
     * @brief Parameters for creating a directional light.
     */
    struct directional_light_desc
    {
        /** @brief Direction vector of the light */
        vec3 direction;                  
        /** @brief Ambient color component */
        color ambient;                    
        /** @brief Diffuse color component */
        color diffuse;                    
        /** @brief Specular color component */
        color specular;                   
        /** @brief Enable specular highlights */
        bool specular_enabled = false;   
        /** @brief Enable shadow casting */
        bool cast_shadows = false;       
    };

    /**
     * @brief Remove all lights from the current scene.
     */
    void no_lights();

    /**
     * @brief Add a new directional light to the scene.
     * @param desc Descriptor struct containing light parameters.
     * @return Identifier of the created directional light.
     */
    light_id directional_light(const directional_light_desc& desc);

    /**
     * @brief Add a new point light to the scene.
     * @param desc Descriptor struct containing light parameters.
     * @return Identifier of the created point light.
     */
    light_id point_light(const point_light_desc& desc);

    /**
     * @brief Update the position of an existing light.
     * @param id Identifier of the light.
     * @param type Type of the light (POINT or DIRECTIONAL).
     * @param x X-coordinate in world space.
     * @param y Y-coordinate in world space.
     * @param z Z-coordinate in world space.
     */
    void light_position(light_id id, light_type type, float x, float y, float z);

    /**
     * @brief Update the direction vector of an existing light.
     * @param id Identifier of the light.
     * @param type Type of the light.
     * @param x X-component of the direction vector.
     * @param y Y-component of the direction vector.
     * @param z Z-component of the direction vector.
     */
    void light_direction(light_id id, light_type type, float x, float y, float z);

    /**
     * @brief Set the ambient color of a light.
     * @param id Identifier of the light.
     * @param type Type of the light.
     * @param r Red channel (0.0 to 1.0).
     * @param g Green channel (0.0 to 1.0).
     * @param b Blue channel (0.0 to 1.0).
     */
    void light_ambient(light_id id, light_type type, float r, float g, float b);

    /**
     * @brief Set the diffuse color of a light.
     * @param id Identifier of the light.
     * @param type Type of the light.
     * @param r Red channel (0.0 to 1.0).
     * @param g Green channel (0.0 to 1.0).
     * @param b Blue channel (0.0 to 1.0).
     */
    void light_diffuse(light_id id, light_type type, float r, float g, float b);

    /**
     * @brief Set the specular color of a light.
     * @param id Identifier of the light.
     * @param type Type of the light.
     * @param r Red channel (0.0 to 1.0).
     * @param g Green channel (0.0 to 1.0).
     * @param b Blue channel (0.0 to 1.0).
     */
    void light_specular(light_id id, light_type type, float r, float g, float b);

    /**
     * @brief Enable or disable specular highlights on a light.
     * @param id Identifier of the light.
     * @param type Type of the light.
     * @param enable True to enable specular, false to disable.
     */
    void light_enable_specular(light_id id, light_type type, bool enable);

    /**
     * @brief Enable or disable shadow casting for a light.
     * @param id Identifier of the light.
     * @param type Type of the light.
     * @param enable True to enable shadows, false to disable.
     */
    void light_enable_shadows(light_id id, light_type type, bool enable);

} // namespace ppp
