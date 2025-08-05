/**
 * @file transform.h
 * @brief Coordinate system transformation stack and operations.
 */
#pragma once

#include "vector.h"
#include "matrix.h"
#include "quat.h"

namespace ppp
{
    /**
     * @brief Pushes the current transformation matrix onto the stack.
     */
    void push();

    /**
     * @brief Pops the top transformation matrix from the stack.
     */
    void pop();

    /**
     * @brief Retrieves the active transformation matrix.
     * @return Current world transform as a mat4.
     */
    mat4 active_transform();

    /**
     * @brief Rotates the coordinate system by a given angle about the origin.
     * @param angle Rotation angle (interpreted according to angle_mode()).
     */
    void rotate(float angle);

    /**
     * @brief Rotates the coordinate system around an arbitrary axis.
     * @param axisx X component of rotation axis.
     * @param axisy Y component of rotation axis.
     * @param axisz Z component of rotation axis.
     * @param angle Rotation angle (interpreted according to angle_mode()).
     */
    void rotate(float axisx, float axisy, float axisz, float angle);

    /**
     * @brief Rotates the coordinate system around the specified axis vector.
     * @param axis Rotation axis as a vec3.
     * @param angle Rotation angle (interpreted according to angle_mode()).
     */
    void rotate(const vec3& axis, float angle);

    /**
     * @brief Scales the coordinate system in X and Y.
     * @param x Scale factor in X.
     * @param y Scale factor in Y.
     */
    void scale(float x, float y);

    /**
     * @brief Scales the coordinate system in X, Y, and Z.
     * @param x Scale factor in X.
     * @param y Scale factor in Y.
     * @param z Scale factor in Z.
     */
    void scale(float x, float y, float z);

    /**
     * @brief Scales the coordinate system by a 2D vector.
     * @param scale Scale factors as a vec2.
     */
    void scale(const vec2& scale);

    /**
     * @brief Scales the coordinate system by a 3D vector.
     * @param scale Scale factors as a vec3.
     */
    void scale(const vec3& scale);

    /**
     * @brief Translates the coordinate system in X and Y.
     * @param x Translation in X.
     * @param y Translation in Y.
     */
    void translate(float x, float y);

    /**
     * @brief Translates the coordinate system in X, Y, and Z.
     * @param x Translation in X.
     * @param y Translation in Y.
     * @param z Translation in Z.
     */
    void translate(float x, float y, float z);

    /**
     * @brief Translates the coordinate system by a 2D vector.
     * @param trans Translation vector as a vec2.
     */
    void translate(const vec2& trans);

    /**
     * @brief Translates the coordinate system by a 3D vector.
     * @param trans Translation vector as a vec3.
     */
    void translate(const vec3& trans);

    /**
     * @brief Applies a composite transformation consisting of translation, rotation, and scale.
     * 
     * This function translates the coordinate system by the given position, then rotates it
     * around the axis-angle derived from the quaternion, and finally scales it.
     *
     * @param position Position vector to translate by.
     * @param rotation Rotation as a quaternion.
     * @param scale Scale factors as a vec3.
     */
    void transform(const vec3& position, const quat& rotation, const vec3& scale);

    /**
     * @brief Applies a composite transformation consisting of translation, rotation, and scale.
     * 
     * This function translates the coordinate system by the given position, then rotates it
     * around the axis-angle derived from the quaternion, and finally scales it.
     *
     * @param position Position vector to translate by.
     * @param rotation Rotation as a float.
     * @param scale Scale factors as a vec2.
     */
    void transform(const vec2& position, float rotation, const vec2& scale);
}