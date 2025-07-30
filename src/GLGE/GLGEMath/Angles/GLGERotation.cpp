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

s_Rotation::s_Rotation(const vec3& dir) noexcept
{
    double angle = sqrt((1. + dot(GLGE_ROTATION_GLOBAL_FORWARD, dir)) * 0.5);
    m_quat = Quaternion(angle, cross(GLGE_ROTATION_GLOBAL_FORWARD, dir) / (dir.length() * 2. * angle));
    m_quat.j *= -1.;
}

s_Rotation::s_Rotation(const Euler& euler) noexcept
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

vec3 s_Rotation::rotate(const vec3& vector) const noexcept
{
    //compute the quaternion
    Quaternion quat = m_quat * vector * -m_quat;
    //return the imaginary part as a vector
    return vec3(quat.i, quat.j, quat.k);
}

dvec3 s_Rotation::rotate(const dvec3& vector) const noexcept
{
    //compute the quaternion
    Quaternion quat = m_quat * vector * -m_quat;
    //return the imaginary part as a vector
    return dvec3(quat.i, quat.j, quat.k);
}

Euler s_Rotation::getEulerRotation() const noexcept
{
    //source (edited to fit the class): https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_code_2

    //store the angles to return
    Euler angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (m_quat.w * m_quat.i + m_quat.j * m_quat.k);
    double cosr_cosp = 1 - 2 * (m_quat.i * m_quat.i + m_quat.j * m_quat.j);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp) * GLGE_RADIANS_TO_DEGREES;

    // pitch (y-axis rotation)
    double sinp = std::sqrt(1 + 2 * (m_quat.w * m_quat.j - m_quat.i * m_quat.k));
    double cosp = std::sqrt(1 - 2 * (m_quat.w * m_quat.j - m_quat.i * m_quat.k));
    angles.pitch = (2 * std::atan2(sinp, cosp) - M_PI / 2) * GLGE_RADIANS_TO_DEGREES;

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (m_quat.w * m_quat.k + m_quat.i * m_quat.j);
    double cosy_cosp = 1 - 2 * (m_quat.j * m_quat.j + m_quat.k * m_quat.k);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp) * GLGE_RADIANS_TO_DEGREES;

    //return and re-format for the correct order
    return Euler(angles.roll, angles.pitch, angles.yaw);
}

mat4 s_Rotation::getUVNMatrix() const noexcept
{
    //compute the forward, left and right according to the rotation
    vec3 f = rotate(GLGE_ROTATION_GLOBAL_FORWARD);
    vec3 r = rotate(GLGE_ROTATION_GLOBAL_RIGHT);
    vec3 u = rotate(GLGE_ROTATION_GLOBAL_UP);

    //return the UVN matrix constructed from the rotated vectors
    return mat4(
        r.x, r.y, r.z, 0,
        u.x, u.y, u.z, 0,
        f.x, f.y, f.z, 0,
        0,   0,   0,   1
    );
}