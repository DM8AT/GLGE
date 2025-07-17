/**
 * @file GLGEQuaternion.h
 * @author DM8AT
 * @brief quaternions are four dimensional values represented by complex numbers. They are mainly used for rotations. 
 * @version 0.1
 * @date 2025-07-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_QUATERNION_
#define _GLGE_QUATERNION_

//include the settings
#include "../../GLGECore/GLGESettings.h"

//include float vectors
#include "../Vectors/GLGEFloatVec.h"
//include double vectors
#include "../Vectors/GLGEDoubleVec.h"

//make the stuff accessible in C
#if GLGE_CPP
extern "C" {
#endif

/**
 * @brief store a single quaternion
 * 
 * Quaternions are usually used for rotations since they don't have a problem knwon as "Gimbal lock" wich can break euler angles
 */
typedef struct s_Quaternion
{

    /**
     * @brief the real value of the quaternion
     */
    double w;
    /**
     * @brief store the imaginary values for the quaternion
     */
    double i, j, k;

    //check for C++ for the member functions
    #if GLGE_CPP

    /**
     * @brief Construct a new Quaternion
     * Just zero everything out
     */
    s_Quaternion() : w(0), i(0), j(0), k(0) {}

    /**
     * @brief Construct a new Quaternion
     * 
     * @param _w the real value for the quaternion
     * @param _i the value on the complex number line
     * @param _j the value on the imaginary number line perpandicular to the real number line and the complex number line
     * @param _k the value on the imaginary number line perpandicular to the first three number lines
     */
    s_Quaternion(float _w, float _i, float _j, float _k)
     : w(_w), i(_i), j(_j), k(_k)
    {}

    /**
     * @brief Construct a new Quaternion from an angle around an axis
     * Usefull for quaternions used for rotations
     * @param angle the angle to rotate around the axis
     * @param axis the axis to rotate around
     */
    s_Quaternion(float angle, const vec3& axis)
     : w(angle), i(axis.x), j(axis.y), k(axis.z)
    {}

    /**
     * @brief Construct a new Quaternion from a position of a point
     * 
     * @param position the position of the point to create the quaternion from
     */
    s_Quaternion(const vec3& position)
     : w(0), i(position.x), j(position.y), k(position.z)
    {}

    /**
     * @brief Construct a new Quaternion from a position of a point
     * 
     * @param position the position of the point to create the quaternion from
     */
    s_Quaternion(const dvec3& position)
     : w(0), i(position.x), j(position.y), k(position.z)
    {}

    /**
     * @brief Construct a new Quaternion
     * 
     * @param vec a 4D vector to store as a quaternion
     */
    s_Quaternion(const vec4& vec)
     : w(vec.w), i(vec.x), j(vec.y), k(vec.z)
    {}

    /**
     * @brief add two quaternions together and get the sum of both as a new quaternion
     * 
     * @param other 
     * @return s_Quaternion 
     */
    inline s_Quaternion operator+(const s_Quaternion& other) const noexcept
    {
        //return a quaternion where all values are added together
        return s_Quaternion(
            w + other.w,
            i + other.i,
            j + other.j,
            k + other.k
        );
    }

    /**
     * @brief subtract two quaternions from each other
     * 
     * @param other the quaternion to subtract from this quaternion
     * @return s_Quaternion the difference of both quaternions
     */
    inline s_Quaternion operator-(const s_Quaternion& other) const noexcept
    {
        //return a quaternion where all values are subtracted from another
        return s_Quaternion(
            w - other.w,
            i - other.i,
            j - other.j,
            k - other.k
        );
    }

    /**
     * @brief multiply two quaternions together using complex multiplication
     * 
     * @param other the quaternion to multiply with
     * @return s_Quaternion the product of both quaternions
     */
    inline s_Quaternion operator*(const s_Quaternion& other) const noexcept
    {
        //create a quaternion to return
        s_Quaternion out;
        //fill the quaternion with the correct values
        out.w = (w*other.w) - (i*other.i) - (j*other.j) - (k*other.k);
        out.i = (i*other.w) + (w*other.i) + (j*other.k) - (k*other.j);
        out.j = (j*other.w) + (w*other.j) + (k*other.i) - (i*other.k);
        out.k = (k*other.w) + (w*other.k) + (i*other.j) - (j*other.i);
        //return the computed quaternion
        return out;
    }

    /**
     * @brief get a quaternion that inverses this quaternion
     * 
     * @return s_Quaternion a quaternion that inverts this quaternion
     */
    inline s_Quaternion operator-() const noexcept
    {
        //return an inversion of everything except the angle
        return s_Quaternion(w, -i,-j,-k);
    }

    /**
     * @brief get the inverse (conjugated) quaternion
     * 
     * @return Quaternion the inverted quaternion
     */
    inline s_Quaternion conjugate() const noexcept {return -(*this);}

    /**
     * @brief add another quaternion to this quaternion
     * 
     * @param other the quaternion to add to this one
     */
    inline void operator+=(const s_Quaternion& other) noexcept
    {
        //increment all values by the values of the other quaternion
        w += other.w;
        i += other.i;
        j += other.j;
        k += other.k;
    }

    /**
     * @brief add subtract another quaternion from this one
     * 
     * @param other the quaternion to subtract from this one
     */
    inline void operator-=(const s_Quaternion& other) noexcept
    {
        //increment all values by the values of the other quaternion
        w -= other.w;
        i -= other.i;
        j -= other.j;
        k -= other.k;
    }

    /**
     * @brief multiply two quaternions together using complex multiplication
     * 
     * @param other the quaternion to multiply with
     * @return s_Quaternion the product of both quaternions
     */
    s_Quaternion operator*(const s_Quaternion& other) noexcept
    {
        //create a quaternion to return
        s_Quaternion out;
        //fill the quaternion with the correct values
        out.w = (w*other.w) - (i*other.i) - (j*other.j) - (k*other.k);
        out.i = (i*other.w) + (w*other.i) + (j*other.k) - (k*other.j);
        out.j = (j*other.w) + (w*other.j) + (k*other.i) - (i*other.k);
        out.k = (k*other.w) + (w*other.k) + (i*other.j) - (j*other.i);
        //return the computed quaternion
        return out;
    }

    /**
     * @brief multiply a quaternion to this one and store the product in this quaternion
     * 
     * @param other the quaternion to multiply with this one
     */
    void operator*=(const s_Quaternion& other) noexcept
    {
        //create a quaternion to return
        s_Quaternion out;
        //fill the quaternion with the correct values
        out.w = (w*other.w) - (i*other.i) - (j*other.j) - (k*other.k);
        out.i = (i*other.w) + (w*other.i) + (j*other.k) - (k*other.j);
        out.j = (j*other.w) + (w*other.j) + (k*other.i) - (i*other.k);
        out.k = (k*other.w) + (w*other.k) + (i*other.j) - (j*other.i);
        //store the result
        *this = out;
    }

    #endif //end of C++ functions

} Quaternion;

/**
 * @brief add two quaternions together
 * 
 * @param q the first quaternion
 * @param p the other quaternion
 * @return Quaternion the sum of both quaternions
 */
Quaternion GLGE_C_FUNC(quaternion_Add)(const Quaternion q, const Quaternion p);

/**
 * @brief compute the difference of two quaternions
 * 
 * @param q the first quaternion
 * @param p the quaternion to subtract from the first quaternion
 * @return Quaternion the difference of both quaternions
 */
Quaternion GLGE_C_FUNC(quaternion_Subtract)(const Quaternion q, const Quaternion p);

/**
 * @brief multiply two quaternions together
 * 
 * @param q the first quaternion
 * @param p the second quaternion
 * @return Quaternion the product of both quaternions
 */
Quaternion GLGE_C_FUNC(quaternion_Multiply)(const Quaternion q, const Quaternion p);

/**
 * @brief invert the inputed quaternion
 * 
 * @param q the quaternion to invert
 * @return Quaternion the inverted quaternion
 */
Quaternion GLGE_C_FUNC(quaternion_Invert)(const Quaternion q);

/**
 * @brief conjugate the inputed quaternion
 * 
 * @param q the quaternion to conjugate
 * @return Quaternion the conjugated quaternion
 */
inline Quaternion GLGE_C_FUNC(quaternion_Conjugate)(const Quaternion q) {return GLGE_C_FUNC(quaternion_Invert)(q);}

/**
 * @brief add a quaternion to the first quaternion
 * 
 * @param q a pointer to the quaternion to store the sum into and the first value of the addition
 * @param p the quaternion to add to the first quaternion
 */
void GLGE_C_FUNC(quaternion_AddTo)(Quaternion* q, const Quaternion p);

/**
 * @brief subtract a quaternion from the first quaternion
 * 
 * @param q a pointer to the quaternion to store the difference into and the first value of the subtraction
 * @param p the quaternion to subtract from the first quaternion
 */
void GLGE_C_FUNC(quaternion_SubtractFrom)(Quaternion* q, const Quaternion p);

//end a potential C section
#if GLGE_CPP
}

/**
 * @brief print an instance of the quaternion into an output stream
 * 
 * @param os the output stream to print to
 * @param q the quaternion to print
 * @return std::ostream& the filled output stream
 */
inline std::ostream& operator<<(std::ostream& os, const s_Quaternion& q) noexcept
{
    //fill out the output stream and return it
    return os << "quaternion{w: " << q.w << ", i: " << q.i << ", j: " << q.j << ", k: " << q.k << "}";
}

#endif //end of C++ section

#endif