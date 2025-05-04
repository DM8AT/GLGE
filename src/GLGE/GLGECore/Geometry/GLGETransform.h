/**
 * @file GLGETransform.h
 * @author DM8AT
 * @brief define the layout of a transformation for GLGE
 * @version 0.1
 * @date 2025-04-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_TRANSFORM_
#define _GLGE_TRANSFORM_

//include the settings and utilities
#include "../GLGEUtil.h"
//include 3D vectors
#include "../../GLGEMath/Vectors/GLGEFloatVec.h"
//include euler angles
#include "../../GLGEMath/Angles/GLGEEuler.h"

/**
 * @brief define the layout of a transform
 */
typedef struct s_Transform {
    /**
     * @brief store the position of the transformation
     */
    vec3 pos;
    /**
     * @brief store the rotation of the transform
     */
    Euler rot;
    /**
     * @brief store the scaling of the transform
     */
    vec3 scale
    //check for C++
    #if GLGE_CPP
    //if this is C++, make the default 1
     = vec3(1)
    #endif //C++ section
    ;

    //check for C++
    #if GLGE_CPP

    /**
     * @brief Construct a new Transform
     */
    s_Transform() = default;

    /**
     * @brief Construct a new Transform
     * 
     * @param _pos the position of the object in 3D space
     * @param _rot the 
     * @param _scale 
     */
    s_Transform(const vec3& _pos, const Euler& _rot = vec3(0), const vec3& _scale = vec3(1))
     : pos(_pos), rot(_rot), scale(_scale)
    {}

    /**
     * @brief print the transform to an output stream
     * 
     * @param os the output stream to print to
     * @param t the transform to print
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const s_Transform& t) noexcept {return os << "transform{pos: " << t.pos << ", rot: " << t.rot << ", scale: " << t.scale << "}";}

    #endif //C++ section

} Transform;

#endif