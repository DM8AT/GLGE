/**
 * @file GLGESphere.cpp
 * @author DM8AT
 * @brief implement all functions for the spheres that are not inlined
 * @version 0.1
 * @date 2025-06-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the spheres
#include "GLGESphere.h"
//include AABBs
#include "GLGE_AABB.h"
//include math
#include <math.h>

Sphere::Sphere(const vec3* positions, uint64_t posCount)
 : m_pos(0), m_radius(0)
{
    //I am using a simple algorithem, not really the fastest. Not really a problem for initalization. 
    //Algorithem: 
    // - First, compute an AABB for the point cloud
    // - Then, compute the center of the AABB
    // - use the distance from the center to one edge as the radius

    //compute the AABB
    AABB aabb = AABB(positions, posCount);
    //the position of the sphere is the average of the minimum and maximum
    m_pos = (aabb.getMin() + aabb.getMax()) / 2.;
    //compute the distance between center and minimum and use it as the radius
    vec3 diff = aabb.getMin() - m_pos;
    m_radius = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
}

float Sphere::getSqrDistanceFrom(const vec3& point) const noexcept
{
    //calculate the squared distance between the centers
    float sqrDist = (point.x - m_pos.x) * (point.x - m_pos.x) + (point.y - m_pos.y) * (point.y - m_pos.y) + (point.z - m_pos.z) * (point.z - m_pos.z);
    //correct the distance by subtracting the squared radius
    sqrDist -= m_radius*m_radius;
    //return the squared distance
    return sqrDist;
}

//C-Function maps

//A function map is as follows: Cast to the C++ type and then execute the desired functionallity
//if a sphere object is returned, cast it to the C type before returning it. 

Sphere_t* GLGE_C_FUNC(sphere_Create)(const vec3* pos, float radius) {return (Sphere_t*)(new Sphere(*pos, radius));}

Sphere_t* GLGE_C_FUNC(sphere_FromPoints)(const vec3* points, uint64_t pointCount) {return (Sphere_t*)(new Sphere(points, pointCount));}

void GLGE_C_FUNC(sphere_Destroy)(Sphere_t* sphere) {delete ((Sphere*)sphere);}

const vec3* GLGE_C_FUNC(sphere_getPos)(const Sphere_t* sphere) {return &((Sphere*)sphere)->getPos();}

float GLGE_C_FUNC(sphere_getRadius)(const Sphere_t* sphere) {return ((Sphere*)sphere)->getRadius();}

void GLGE_C_FUNC(sphere_setPos)(const vec3* pos, Sphere_t* sphere) {((Sphere*)sphere)->setPos(*pos);}

void GLGE_C_FUNC(sphere_setRadius)(float radius, Sphere_t* sphere) {((Sphere*)sphere)->setRadius(radius);}

float GLGE_C_FUNC(sphere_getSquaredRadius)(const Sphere* sphere) {return ((Sphere*)sphere)->getSquaredRadius();}

void GLGE_C_FUNC(sphere_translate)(const vec3* delta, Sphere_t* sphere) {((Sphere*)sphere)->translate(*delta);}

void GLGE_C_FUNC(sphere_scaleUniform)(float scale, Sphere_t* sphere) {((Sphere*)sphere)->scale(scale);}

void GLGE_C_FUNC(sphere_scale)(const vec3* scale, Sphere_t* sphere) {((Sphere*)sphere)->scale(*scale);}

float GLGE_C_FUNC(sphere_getSqrDistanceFrom)(const vec3* point, const Sphere_t* sphere) {return ((Sphere*)sphere)->getSqrDistanceFrom(*point);}

float GLGE_C_FUNC(sphere_getDistanceFrom)(const vec3* point, const Sphere_t* sphere) {return ((Sphere*)sphere)->getDistanceFrom(*point);}