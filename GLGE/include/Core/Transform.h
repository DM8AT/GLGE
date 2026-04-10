/**
 * @file Transform.h
 * @author DM8AT
 * @brief define what a transform component is
 * @version 0.1
 * @date 2026-03-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_TRANSFORM_
#define _GLGE_CORE_TRANSFORM_

//add common stuff
#include "Common.h"

/**
 * @brief use the libraries namespace
 */
namespace GLGE {

    /**
     * @brief a transform stores the position, rotation and scaling
     */
    struct Transform {
        /**
         * @brief store the position
         */
        vec3 pos;
        /**
         * @brief store the rotation
         */
        Quaternion rot;
        /**
         * @brief store the scale
         */
        vec3 scale;
    };

    /**
     * @brief store a 2 dimensional transform
     */
    struct Transform2D {
        /**
         * @brief the position in 2D space
         */
        vec2 pos;
        /**
         * @brief the angle of the object
         */
        float angle;
        /**
         * @brief the scale of the object
         */
        vec2 scale;
    };

}

#endif