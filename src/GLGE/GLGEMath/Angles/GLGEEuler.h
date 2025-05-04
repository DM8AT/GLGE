/**
 * @file GLGEEuler.h
 * @author DM8AT
 * @brief define the layout of euler angles
 * @version 0.1
 * @date 2025-04-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_EULER_
#define _GLGE_EULER_

//include settings and utilities
#include "../../GLGECore/GLGEUtil.h"
//include float vectors
#include "../Vectors/GLGEFloatVec.h"

//check if this is C++
#if GLGE_CPP
//define a default value of 0 for elements
#define __GLGE_EULER_DEFAULT = 0
//else
#else
//set the default to empty
#define __GLGE_EULER_DEFAULT
#endif //C++ check

/**
 * @brief define the layout of euler angles
 */
typedef struct s_Euler {
    /**
     * @brief store the yaw of the angle
     */
    float yaw __GLGE_EULER_DEFAULT;
    /**
     * @brief store the pitch of the angle
     */
    float pitch __GLGE_EULER_DEFAULT;
    /**
     * @brief store the roll of the angle
     */
    float roll __GLGE_EULER_DEFAULT;

    //check if this is C++
    #if GLGE_CPP

    /**
     * @brief Construct a new Euler angle
     */
    s_Euler() = default;

    /**
     * @brief Construct a new euler angle
     * 
     * @param angle the angle as a 3D vector
     */
    s_Euler(const vec3& angle) : yaw(angle.x), pitch(angle.y), roll(angle.z) {}

    /**
     * @brief Construct a new euler angle
     * 
     * @param _yaw the yaw of the angle
     * @param _pitch the pitch of the angle
     * @param _roll the roll of the angle
     */
    s_Euler(float _yaw, float _pitch, float _roll) : yaw(_yaw), pitch(_pitch), roll(_roll) {}

    /**
     * @brief print a euler angle into a output stream
     * 
     * @param os the output stream to print to
     * @param e the euler angle to print
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const s_Euler& e) {return os << "eulerAngle{yaw: " << e.yaw << ", pitch: " << e.pitch << ", roll: " << e.roll << "}";}

    #endif //C++ section
} Euler; 

//undefine the default
#undef __GLGE_EULER_DEFAULT

#endif