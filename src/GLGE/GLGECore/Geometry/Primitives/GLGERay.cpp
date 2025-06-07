/**
 * @file GLGERay.cpp
 * @author DM8AT
 * @brief implement the ray class
 * @version 0.1
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the ray class definition
#include "GLGERay.h"

void Ray::refract(const vec3& normal, float qr) noexcept
{
    //refrection formulat taken from: https://stackoverflow.com/questions/29758545/how-to-find-refraction-vector-from-incoming-vector-and-surface-normal
    //compute the inner cosine of the angles
    float cosI = -dot(m_dir, normal);
    //compute the sine of something (some magic)
    float sin2T = qr*qr*(1-cosI*cosI);
    //check if the value is out of range. If it is, set the direction to NaN and stop
    if (sin2T > 1.f) {m_dir = NAN; return;}
    //compute the cosine of this something
    float cosT = sqrt(1. - sin2T);
    //calcualte the new refracted direction
    m_dir = m_dir*qr + normal * (qr*cosI - cosT);

    //re-calculate the ray direction's reciprical
    m_idir = vec3(1)/m_dir;
}