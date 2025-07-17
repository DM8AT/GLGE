/**
 * @file GLGERotation.h
 * @author DM8AT
 * @brief implement a class to wrap around all rotation data
 * @version 0.1
 * @date 2025-07-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ROTATION_
#define _GLGE_ROTATION_

//inlcude euler angles and settings
#include "GLGEEuler.h"
//include quaternions for the actual rotation
#include "../Imaginary/GLGEQuaternion.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a constant to multiply with to convert degrees to radians
 */
#define GLGE_DEGREES_TO_RADIANS 0.01745329

/**
 * @brief store a constant to multiply with to convert radians to degrees
 */
#define GLGE_RADIANS_TO_DEGREES 57.295779

/**
 * @brief store the global vector that points forward in world space
 */
#define GLGE_ROTATION_GLOBAL_FORWARD vec3(0,0,1)

/**
 * @brief store the global vector that points right in world space
 */
#define GLGE_ROTATION_GLOBAL_RIGHT vec3(1,0,0)

/**
 * @brief store the global vector that points up in world space
 */
#define GLGE_ROTATION_GLOBAL_UP vec3(0,1,0)

/**
 * @brief store and convert between conversions
 * @warning quaternions are used as intermeditate format. Use them for fastest access and manipulation
 */
class Rotation
{
public:

    /**
     * @brief Construct a new Rotation instance
     */
    Rotation() = default;

    /**
     * @brief Construct a new Rotation object from euler angles
     * 
     * @param euler the set of euler angles to create the rotation object from
     */
    Rotation(const Euler& euler) noexcept;

    /**
     * @brief Construct a new Rotation object from a direction vector
     * 
     * @param dir a vector that points to the direction a point at (0,0,1) would be rotated to if the rotation is applied
     */
    Rotation(const vec3& dir) noexcept;

    /**
     * @brief Construct a new Rotation object from a quaternion
     * 
     * @param quat the quaternion that describes the rotation
     */
    inline Rotation(const Quaternion& quat) noexcept
     : m_quat(quat)
    {}

    /**
     * @brief rotate an arbituary vector by the stored rotation element
     * 
     * @param vector the 3D vector to rotate by this rotation
     * @return vec3 the rotated 3D vector
     */
    vec3 rotate(const vec3& vector) const noexcept;
    
    /**
     * @brief rotate an arbituary vector by the stored rotation element
     * 
     * @param vector the 3D double vector to rotate by this rotation
     * @return dvec3 the rotated 3D double vector
     */
    dvec3 rotate(const dvec3& vector) const noexcept;

    /**
     * @brief add two rotations together to apply the resulting rotation at once
     * 
     * @param rotation the other rotation object to add to this one
     * @return Rotation the sum of both rotation instances
     */
    Rotation operator+(const Rotation& rotation) const noexcept
    {
        //return the sum of both objects
        return m_quat * rotation.m_quat;
    }

    /**
     * @brief change this rotation by another rotation
     * 
     * @param rot the rotation to change this rotation by
     */
    inline void operator+=(const Rotation& rot) noexcept {m_quat *= rot.m_quat;}

    /**
     * @brief Get the rotation stored in this object as euler angles
     * 
     * @return Euler the set of euler angles that describe the rotation stored in this instance
     */
    Euler getEulerRotation() const noexcept;

    /**
     * @brief Get a vector that points in the direction indicated by the rotation
     * 
     * @return vec3 the normalized vector pointing in the indicated direction
     */
    vec3 getDirectionVector() const noexcept
    {
        //simply rotate the forward vector by the stored quaternion
        return rotate(GLGE_ROTATION_GLOBAL_FORWARD);
    }

    /**
     * @brief Get the rotation stored in this object as a quaternion
     * 
     * @return const Quaternion& the quaternion that describes the stored rotation
     */
    inline const Quaternion& getQuaternionRotation() const noexcept {return m_quat;}

    /**
     * @brief print the rotation to an output stream
     * 
     * @param os the output stream to fill out
     * @param rot a constant reference to the rotation object to print out
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Rotation& rot) noexcept
    {return os << "rotation{quat: " << rot.m_quat << "}";}

protected:

    /**
     * @brief store a quaternion for the rotation
     * Quaternions are a good choise to avoid gimbal lock
     */
    Quaternion m_quat;

};

#endif

#endif