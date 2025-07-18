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
//include double vectors
#include "../Vectors/GLGEDoubleVec.h"

//check if this is C++
#if GLGE_CPP
//define a default value of 0 for elements
#define __GLGE_EULER_DEFAULT = 0

//if this is C++, start a C section
extern "C"
{
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
    double yaw __GLGE_EULER_DEFAULT;
    /**
     * @brief store the pitch of the angle
     */
    double pitch __GLGE_EULER_DEFAULT;
    /**
     * @brief store the roll of the angle
     */
    double roll __GLGE_EULER_DEFAULT;

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
     * @param angle the angle as a 3D vector
     */
    s_Euler(const dvec3& angle) : yaw(angle.x), pitch(angle.y), roll(angle.z) {}

    /**
     * @brief Construct a new euler angle
     * 
     * @param _yaw the yaw of the angle
     * @param _pitch the pitch of the angle
     * @param _roll the roll of the angle
     */
    s_Euler(float _yaw, float _pitch, float _roll) : yaw(_yaw), pitch(_pitch), roll(_roll) {}

    /**
     * @brief add two euler angles together
     * 
     * @param euler the other euler angle to add to this one
     * @return s_Euler 
     */
    inline s_Euler operator+(const s_Euler& euler) const noexcept
    {
        //return the added rotations
        return s_Euler(
            yaw + euler.yaw,
            pitch + euler.pitch,
            roll + euler.roll
        );
    }

    /**
     * @brief subtract another euler angle from this euler angle
     * 
     * @param euler the euler angle to subtract from this euler angle
     * @return s_Euler the difference of this and the other euler angle
     */
    inline s_Euler operator-(const s_Euler& euler) const noexcept
    {
        //return the difference of the rotations
        return s_Euler(
            yaw - euler.yaw,
            pitch - euler.pitch,
            roll - euler.roll
        );
    }

    /**
     * @brief add another euler angle to this euler angle
     * 
     * @param euler the euler angle to add to this one
     */
    inline void operator+=(const s_Euler& euler) noexcept
    {
        //just add the components together and store them in this
        yaw += euler.yaw;
        pitch += euler.pitch;
        roll += euler.roll;
    }

    /**
     * @brief subtract another euler angle from this euler angle
     * 
     * @param euler the euler angle to subtract from this one
     */
    inline void operator-=(const s_Euler& euler) noexcept
    {
        //just subtract all the components and store them in this
        yaw -= euler.yaw;
        pitch -= euler.pitch;
        roll -= euler.roll;
    }

    /**
     * @brief create a set of euler angles that remove the rotation applied by this set of euler angles
     * 
     * @return s_Euler the inverse angle that removes the rotation applied by this euler angle
     */
    inline s_Euler operator-() const noexcept
    {
        //just negate all components
        return s_Euler(-yaw, -pitch, -roll);
    }

    /**
     * @brief format the euler angle as a 3D vector
     * 
     * @return vec3 the 3D vector that describes this euler angle instance
     */
    inline vec3 asVec3() const noexcept {return vec3(yaw, pitch, roll);}

    /**
     * @brief format the euler angle as a 3D vector of doubles
     * 
     * @return dvec3 the 3D double vector that describes this euler angle instance
     */
    inline dvec3 asDVec3() const noexcept {return dvec3(yaw, pitch, roll);}

    #endif //C++ section
} Euler;

/**
 * @brief add two euler angles together and return the sum of them
 * 
 * @param a the first summand
 * @param b the second summand
 * @return Euler the sum of both euler angles
 */
Euler GLGE_C_FUNC(euler_Add)(const Euler a, const Euler b);

/**
 * @brief add a vector 3 to a euler angle. The Vec3 will be interpreted as a euler angle. 
 * 
 * @param a the euler angle and the first summand
 * @param b the vector 3 and the seccond summand
 * @return Euler the sum of the Euler angle and the 3D vector
 */
Euler GLGE_C_FUNC(euler_AddVec3)(const Euler a, const vec3 b);

/**
 * @brief add a double vector 3 to a euler angle. The Vec3 will be interpreted as a euler angle. 
 * 
 * @param a the euler angle and the first summand
 * @param b the double vector 3 and the seccond summand
 * @return Euler the sum of the Euler angle and the 3D vector
 */
Euler GLGE_C_FUNC(euler_AddDVec3)(const Euler a, const dvec3 b);

/**
 * @brief subtract two euler angles and return the difference
 * 
 * @param a the euler angle to subtract the second one from
 * @param b the euler angle to subtract from the first one
 * @return Euler the difference of both euler angles
 */
Euler GLGE_C_FUNC(euler_Subtract)(const Euler a, const Euler b);

/**
 * @brief subtract a 3D vector from a euler angle. The 3D vector will be interpreted as a euler angle. 
 * 
 * @param a the euler angle to subtract the vector 3 from
 * @param b the 3D vector to subtract from the euler angle
 * @return Euler the difference of the euler angle and the 3D vector
 */
Euler GLGE_C_FUNC(euler_SubtractVec3)(const Euler a, const vec3 b);

/**
 * @brief subtract a 3D double vector from a euler angle. The 3D vector will be interpreted as a euler angle. 
 * 
 * @param a the euler angle to subtract the vector 3 from
 * @param b the 3D double vector to subtract from the euler angle
 * @return Euler the difference of the euler angle and the 3D vector
 */
Euler GLGE_C_FUNC(euler_SubtractDVec3)(const Euler a, const dvec3 b);

/**
 * @brief add a euler angle to this euler angle
 * 
 * @param a the euler angle to add the other euler angle to
 * @param b the euler angle to add to the other euler angle
 */
void GLGE_C_FUNC(euler_AddTo)(Euler* a, const Euler b);

/**
 * @brief add a 3D vector to a euler angle. Thee 3D vector will be interpreted as a euler angle. 
 * 
 * @param a the euler angle to add the 3D vector to
 * @param b the 3D vector to interprete as a euler angle and add to the other euler angle
 */
void GLGE_C_FUNC(euler_AddVec3To)(Euler* a, const vec3 b);

/**
 * @brief add a 3D double vector to a euler angle. Thee 3D vector will be interpreted as a euler angle. 
 * 
 * @param a the euler angle to add the 3D double vector to
 * @param b the 3D double vector to interprete as a euler angle and add to the other euler angle
 */
void GLGE_C_FUNC(euler_AddDVec3To)(Euler* a, const dvec3 b);

/**
 * @brief subtract a euler angle from another euler angle and store the difference in the first euler angle
 * 
 * @param a the euler angle to subtract the other from and to store the difference into
 * @param b the euler angle to subtract from the first euler angle
 */
void GLGE_C_FUNC(euler_SubtractFrom)(Euler* a, const Euler b);

/**
 * @brief subtract a 3D vector from a euler angle and store the resulting difference in the euler angle
 * 
 * @param a the euler angle to subtract the 3D vector from and to store the difference into
 * @param b the 3D vector to subtract from the euler angle. The vector will be interpreted as a euler angle. 
 */
void GLGE_C_FUNC(euler_SubtractVec3From)(Euler* a, const vec3 b);

/**
 * @brief subtract a 3D double vector from a euler angle and store the resulting difference in the euler angle
 * 
 * @param a the euler angle to subtract the 3D double vector from and to store the difference into
 * @param b the 3D double vector to subtract from the euler angle. The vector will be interpreted as a euler angle. 
 */
void GLGE_C_FUNC(euler_SubtractDVec3From)(Euler* a, const dvec3 b);

/**
 * @brief invert the inputed input vector. This means if both rotations are applied together they will cancel out. 
 * 
 * @param euler the euler angle to invert
 * @return Euler the inverted euler angle
 */
Euler GLGE_C_FUNC(euler_invert)(const Euler euler);

/**
 * @brief convert a euler angle to a 3D vector. The angles will be stored as follows: x -> yaw | y -> pitch | z -> roll
 * 
 * @param euler the euler angle to convert to a 3D vector
 * @return vec3 the 3D vector the euler angles are stored in
 */
vec3 GLGE_C_FUNC(euler_asVec3)(const Euler euler);

/**
 * @brief convert a euler angle to a 3D double vector. The angles will be stored as follows: x -> yaw | y -> pitch | z -> roll
 * 
 * @param euler the euler angle to convert to a 3D double vector
 * @return dvec3 the 3D double vector the euler angles are stored in
 */
dvec3 GLGE_C_FUNC(euler_asDVec3)(const Euler euler);

//undefine the default
#undef __GLGE_EULER_DEFAULT

//check for C++ to end a potential C section
#if GLGE_CPP
} //end of C section

/**
 * @brief print a euler angle into a output stream
 * 
 * @param os the output stream to print to
 * @param e the euler angle to print
 * @return std::ostream& the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, const s_Euler& e) {return os << "eulerAngle{yaw: " << e.yaw << ", pitch: " << e.pitch << ", roll: " << e.roll << "}";}

#endif //end of C++ section

#endif