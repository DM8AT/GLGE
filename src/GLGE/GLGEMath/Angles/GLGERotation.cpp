/**
 * @file GLGERotation.cpp
 * @author DM8AT
 * @brief implement the not inline functions for the rotation class
 * @version 0.1
 * @date 2025-07-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the rotation class
#include "GLGERotation.h"

Rotation::Rotation(const vec3& dir) noexcept
{
    double angle = sqrt((1. + dot(GLGE_ROTATION_GLOBAL_FORWARD, dir)) * 0.5);
    m_quat = Quaternion(angle, cross(GLGE_ROTATION_GLOBAL_FORWARD, dir) / (dir.length() * 2. * angle));
    m_quat.j *= -1.;
}

Rotation::Rotation(const Euler& euler) noexcept
{
    //store the half euler
    dvec3 hEuler = euler.asDVec3() * 0.5 * GLGE_DEGREES_TO_RADIANS;
    //store the consine and sine of the vector elements
    dvec3 coses = dvec3(cos(hEuler.x), cos(hEuler.y), cos(hEuler.z));
    dvec3 sines = dvec3(sin(hEuler.x), sin(hEuler.y), sin(hEuler.z));
    //convert the euler angle to a quaternion
    m_quat.w = coses.x*coses.y*coses.z + sines.x*sines.y*sines.z;
    m_quat.i = sines.x*coses.y*coses.z - coses.x*sines.y*sines.z;
    m_quat.j = coses.x*sines.y*coses.z + sines.x*coses.y*sines.z;
    m_quat.k = coses.x*coses.y*sines.z - sines.x*sines.y*coses.z;
}

vec3 Rotation::rotate(const vec3& vector) const noexcept
{
    //compute the quaternion
    Quaternion quat = m_quat * vector * -m_quat;
    //return the imaginary part as a vector
    return vec3(quat.i, quat.j, quat.k);
}

dvec3 Rotation::rotate(const dvec3& vector) const noexcept
{
    //compute the quaternion
    Quaternion quat = m_quat * vector * -m_quat;
    //return the imaginary part as a vector
    return dvec3(quat.i, quat.j, quat.k);
}

Euler Rotation::getEulerRotation() const noexcept
{
    //source (edited to fit the class): https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_code_2

    //store the angles to return
    Euler angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2. * (m_quat.w* m_quat.i+ m_quat.j* m_quat.k);
    double cosr_cosp = 1. - 2. * (m_quat.i* m_quat.i+ m_quat.j* m_quat.j);
    angles.roll = -atan2(sinr_cosp, cosr_cosp) * GLGE_RADIANS_TO_DEGREES;

    // pitch (y-axis rotation)
    double sinp = 2. * (m_quat.w* m_quat.j- m_quat.k* m_quat.i);
    if (abs(sinp) >= 1.)
        angles.pitch = copysign(M_PI * 0.5, sinp) * GLGE_RADIANS_TO_DEGREES; // use 90 degrees if out of range
    else
        angles.pitch = asin(sinp) * GLGE_RADIANS_TO_DEGREES;

    // yaw (z-axis rotation)
    double siny_cosp = 2. * (m_quat.w* m_quat.k+ m_quat.i* m_quat.j);
    double cosy_cosp = 1. - 2. * (m_quat.j* m_quat.j+ m_quat.k* m_quat.k);
    angles.yaw = atan2(siny_cosp, cosy_cosp) * GLGE_RADIANS_TO_DEGREES;

    //return and re-format for the correct order
    return Euler(angles.roll, angles.pitch, angles.yaw);

}