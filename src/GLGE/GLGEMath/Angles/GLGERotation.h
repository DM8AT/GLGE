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
//include float matricies
#include "../Matricies/GLGEFloatMatricies.h"
//include quaternions for the actual rotation
#include "../Imaginary/GLGEQuaternion.h"

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
typedef struct s_Rotation
{
    //check for C++
    #if GLGE_CPP

    //use a class-like structure

    /**
     * @brief Construct a new Rotation instance
     */
    s_Rotation() = default;

    /**
     * @brief Construct a new Rotation object from euler angles
     * 
     * @param euler the set of euler angles to create the rotation object from
     */
    s_Rotation(const Euler& euler) noexcept;

    /**
     * @brief Construct a new Rotation object from a direction vector
     * 
     * @param dir a vector that points to the direction a point at (0,0,1) would be rotated to if the rotation is applied
     */
    s_Rotation(const vec3& dir) noexcept;

    /**
     * @brief Construct a new Rotation instance from a 4x4 matrix
     * 
     * @param matrix a constant reference to a 4x4 matrix for the rotation
     */
    s_Rotation(const mat4& matrix) noexcept
    //just use the 3x3 matrix function
     : s_Rotation(
        mat3(matrix.m[0].x, matrix.m[0].y, matrix.m[0].z,
             matrix.m[1].x, matrix.m[1].y, matrix.m[1].z,
             matrix.m[2].x, matrix.m[2].y, matrix.m[2].z)
     )
    {}

    /**
     * @brief Construct a new Rotation instance from a 3x3 matrix
     * 
     * @param matrix a constant reference to a 3x3 matrix storing the rotation
     */
    inline s_Rotation(const mat3& matrix) noexcept
    {
        //store the w element for the quaternion
        double w = sqrt(1. + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2]) * 0.5;
        //pre-compute the denominaotr
        double d = 1. / (4. * w);
        //calculate the quaternion
        m_quat = Quaternion(
            w,
            (matrix.m[2][1] - matrix.m[1][2]) * d,
            (matrix.m[0][2] - matrix.m[2][0]) * d,
            (matrix.m[1][0] - matrix.m[0][1]) * d
        );
    }

    /**
     * @brief Construct a new Rotation object from a quaternion
     * 
     * @param quat the quaternion that describes the rotation
     */
    inline s_Rotation(const Quaternion& quat) noexcept
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
    s_Rotation operator+(const s_Rotation& rotation) const noexcept
    {
        //return the sum of both objects
        return m_quat * rotation.m_quat;
    }

    /**
     * @brief change this rotation by another rotation
     * 
     * @param rot the rotation to change this rotation by
     */
    inline void operator+=(const s_Rotation& rot) noexcept {m_quat *= rot.m_quat;}

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
     * @brief Get a 3x3 matrix that can be used to apply this rotation to an object
     * 
     * @return mat3 the 3x3 matrix to apply this rotation
     */
    inline mat3 getMat3() const noexcept
    {
        /**
         * Source: https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
         * 
         * Structure: 
         * 1 - 2*qy² - 2*qz² 	2*qx*qy - 2*qz*qw 	2*qx*qz + 2*qy*qw
         * 2*qx*qy + 2*qz*qw 	1 - 2*qx² - 2*qz² 	2*qy*qz - 2*qx*qw
         * 2*qx*qz - 2*qy*qw 	2*qy*qz + 2*qx*qw 	1 - 2*qx² - 2*qy²
         */
        return mat3(
            1. - 2.*m_quat.j*m_quat.j - 2.*m_quat.k*m_quat.k, 2.*m_quat.i*m_quat.j - 2.*m_quat.k*m_quat.w, 2.*m_quat.i*m_quat.k + 2.*m_quat.j*m_quat.w,
            2.*m_quat.i*m_quat.j + 2.*m_quat.k*m_quat.w, 1. - 2.*m_quat.i*m_quat.i - 2.*m_quat.k*m_quat.k, 2.*m_quat.j*m_quat.k - 2.*m_quat.i*m_quat.w,
            2.*m_quat.i*m_quat.k - 2*m_quat.j*m_quat.w, 2*m_quat.j*m_quat.k + 2*m_quat.i*m_quat.w, 1. - 2.*m_quat.i*m_quat.i - 2.*m_quat.j*m_quat.j
        );
    }

    /**
     * @brief Get a 4x4 matrix that can be used to apply this rotation on an object
     * 
     * @return mat4 the 4x4 matrix to apply this rotation
     */
    inline mat4 getMat4() const noexcept
    {
        //the same as for 3x3 matricies, just padded with a layer
        return mat4(
            1. - 2.*m_quat.j*m_quat.j - 2.*m_quat.k*m_quat.k, 2.*m_quat.i*m_quat.j - 2.*m_quat.k*m_quat.w, 2.*m_quat.i*m_quat.k + 2.*m_quat.j*m_quat.w, 0,
            2.*m_quat.i*m_quat.j + 2.*m_quat.k*m_quat.w, 1. - 2.*m_quat.i*m_quat.i - 2.*m_quat.k*m_quat.k, 2.*m_quat.j*m_quat.k - 2.*m_quat.i*m_quat.w, 0,
            2.*m_quat.i*m_quat.k - 2*m_quat.j*m_quat.w, 2*m_quat.j*m_quat.k + 2*m_quat.i*m_quat.w, 1. - 2.*m_quat.i*m_quat.i - 2.*m_quat.j*m_quat.j,    0,
            0, 0, 0, 1
        );
    }

    /**
     * @brief print the rotation to an output stream
     * 
     * @param os the output stream to fill out
     * @param rot a constant reference to the rotation object to print out
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const s_Rotation& rot) noexcept
    {return os << "rotation{quat: " << rot.m_quat << "}";}

protected:

    //end of C++ - specific section
    #endif

    /**
     * @brief store a quaternion for the rotation
     * Quaternions are a good choise to avoid gimbal lock
     */
    Quaternion m_quat;

} Rotation;

#endif