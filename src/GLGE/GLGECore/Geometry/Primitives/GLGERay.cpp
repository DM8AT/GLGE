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

//C-Function maps

//A function map is as follows: Cast to the C++ type and then execute the desired functionallity
//if a sphere object is returned, cast it to the C type before returning it. 

Ray_t* GLGE_C_FUNC(ray_Create)(const vec3* origen, const vec3* dir) {return (Ray_t*)(new Ray(*origen, *dir));}

void GLGE_C_FUNC(ray_Destroy)(Ray_t* ray) {delete ((Ray*)ray);}

const vec3* GLGE_C_FUNC(ray_getOrigen)(const Ray_t* ray) {return &((Ray*)ray)->getOrigen();}

const vec3* GLGE_C_FUNC(ray_getDir)(const Ray_t* ray) {return &((Ray*)ray)->getDir();}

const vec3* GLGE_C_FUNC(ray_getRecpDir)(const Ray_t* ray) {return &((Ray*)ray)->getRecipDir();}

void GLGE_C_FUNC(ray_setOrigen)(const vec3* origen, Ray_t* ray) {((Ray*)ray)->setOrigen(*origen);}

void GLGE_C_FUNC(ray_setDir)(const vec3* dir, Ray_t* ray) {((Ray*)ray)->setDir(*dir);}

void GLGE_C_FUNC(ray_reflect)(const vec3* normal, Ray_t* ray) {((Ray*)ray)->reflect(*normal);}

void GLGE_C_FUNC(ray_refract)(const vec3* normal, float qr, Ray_t* ray) {((Ray*)ray)->refract(*normal, qr);}

void GLGE_C_FUNC(ray_stepForward)(float length, Ray_t* ray) {((Ray*)ray)->stepForward(length);}