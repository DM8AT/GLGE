/**
 * @file GLGE_AABB.cpp
 * @author DM8AT
 * @brief implement all non-inline functions for axis alligned bounding boxes
 * @version 0.1
 * @date 2025-06-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include AABBs
#include "GLGE_AABB.h"

AABB::AABB(const vec3* positions, uint64_t posCount) noexcept
{
    //store the first element as the minimum and maximum
    m_min = positions[0];
    m_max = positions[0];
    //iterate over the rest to search the per element minimum and maximum
    for (uint64_t i = 1; i < posCount; ++i)
    {
        //store the per-axis minimum
        m_min.x = (m_min.x < positions[i].x) ? m_min.x : positions[i].x; 
        m_min.y = (m_min.y < positions[i].y) ? m_min.y : positions[i].y; 
        m_min.z = (m_min.z < positions[i].z) ? m_min.z : positions[i].z;
        //store the per-axis maximum
        m_max.x = (m_max.x > positions[i].x) ? m_max.x : positions[i].x;
        m_max.y = (m_max.y > positions[i].y) ? m_max.y : positions[i].y;
        m_max.z = (m_max.z > positions[i].z) ? m_max.z : positions[i].z;
    }
}

void AABB::translate(const vec3& delta) noexcept
{
    //simply add the delta to both minima and maxima
    m_min += delta;
    m_max += delta;
}

void AABB::scale(const vec3& scale) noexcept
{
    //simply multiply both minimum and maximum by the scale
    m_min *= scale;
    m_max *= scale;
}

void AABB::invertThis() noexcept
{
    //store a the old minimum
    vec3 o_min = m_min;
    //swap minimum and maximum
    m_min = m_max;
    m_max = o_min;
}

void AABB::combineToThis(const AABB& other) noexcept
{
    //compute the per-axis minimum of both minimas
    m_min.x = (m_min.x < other.m_min.x) ? m_min.x : other.m_min.x; 
    m_min.y = (m_min.y < other.m_min.y) ? m_min.y : other.m_min.y; 
    m_min.z = (m_min.z < other.m_min.z) ? m_min.z : other.m_min.z;
    //compute the per-axis maximum of both maximas
    m_max.x = (m_max.x > other.m_max.x) ? m_max.x : other.m_max.x;
    m_max.y = (m_max.y > other.m_max.y) ? m_max.y : other.m_max.y;
    m_max.z = (m_max.z > other.m_max.z) ? m_max.z : other.m_max.z;
}

void AABB::uniteToThis(const AABB& other) noexcept
{
    //compute the per-axis maximum of both minimas
    m_min.x = (m_min.x > other.m_min.x) ? m_min.x : other.m_min.x; 
    m_min.y = (m_min.y > other.m_min.y) ? m_min.y : other.m_min.y; 
    m_min.z = (m_min.z > other.m_min.z) ? m_min.z : other.m_min.z;
    //compute the per-axis minimum of both maximas
    m_max.x = (m_max.x < other.m_max.x) ? m_max.x : other.m_max.x;
    m_max.y = (m_max.y < other.m_max.y) ? m_max.y : other.m_max.y;
    m_max.z = (m_max.z < other.m_max.z) ? m_max.z : other.m_max.z;
}

float AABB::getSqrDistanceFrom(const vec3& point) const noexcept
{
    //store the squared distance
    float sqrDist = 0;
    //a temporary value to store the difference
    float diff = 0;
    //on each axis first check if the point is below or above the box on that axis
    //if the side is found, compute the difference
    //lastly, square it and add it to the total
    //X-Axis
    if (point.x < m_min.x) {diff = (m_min.x - point.x);}
    else if (point.x > m_max.x) {diff = (point.x - m_max.x);}
    sqrDist += diff*diff;
    //Y-Axis
    if (point.y < m_min.y) {diff = (m_min.y - point.y);}
    else if (point.y > m_max.y) {diff = (point.y - m_max.y);}
    sqrDist += diff*diff;
    //Z-Axis
    if (point.z < m_min.z) {diff = (m_min.z - point.z);}
    else if (point.z > m_max.z) {diff = (point.z - m_max.z);}
    sqrDist += diff*diff;

    //return the squared distance
    return sqrDist;
}





//C Function mappings

AABB_t* GLGE_C_FUNC(aabb_Create)(const vec3* min, const vec3* max)
{
    //create the AABB class and cast to the C type
    return (AABB_t*)(new AABB(*min, *max));
}

AABB_t* GLGE_C_FUNC(aabb_FromPoints)(const vec3* points, uint64_t pointCount)
{
    //create the AABB instance and cast to the C type
    return (AABB_t*)(new AABB(points, pointCount));
}

void GLGE_C_FUNC(aabb_Destroy)(AABB_t* aabb)
{
    //cast and delete the object
    delete ((AABB*)aabb);
}

//The functions below are simple pass-throughs. They always cast and then execute a function on the C++ object. 

const vec3* GLGE_C_FUNC(aabb_getMin)(const AABB_t* aabb) {return &((AABB*)aabb)->getMin();}

const vec3* GLGE_C_FUNC(aabb_getMax)(const AABB_t* aabb) {return &((AABB*)aabb)->getMax();}

void GLGE_C_FUNC(aabb_setMin)(const vec3* min, AABB_t* aabb) {((AABB*)aabb)->setMin(*min);}

void GLGE_C_FUNC(aabb_setMax)(const vec3* max, AABB_t* aabb) {((AABB*)aabb)->setMax(*max);}

void GLGE_C_FUNC(aabb_translate)(const vec3* delta, AABB_t* aabb) {((AABB*)aabb)->translate(*delta);}

void GLGE_C_FUNC(aabb_scale)(const vec3* scale, AABB_t* aabb) {((AABB*)aabb)->scale(*scale);}

AABB_t* GLGE_C_FUNC(aabb_combine)(const AABB_t* first, const AABB_t* other)
{
    //create the new AABB
    //must be a pointer else the object is destroyed when the function finishes
    //as it would move out of scope
    AABB* ret = new AABB();
    //set the AABB
    *ret = ((AABB*)first)->combine(*((AABB*)other));
    //return the new AABB as the C type
    return (AABB_t*)ret;
}

void GLGE_C_FUNC(aabb_combineToFirst)(AABB_t* first, const AABB_t* other) {((AABB*)first)->combineToThis(*((AABB*)other));}

AABB_t* GLGE_C_FUNC(aabb_unite)(const AABB_t* first, const AABB_t* other)
{
    //create the new AABB
    //for an explenation why it's a pointer, see aabb_combine
    AABB* ret = new AABB();
    //set the AABB
    *ret = ((AABB*)first)->unite(*((AABB*)other));
    //return the new AABB as the C type
    return (AABB_t*)ret;
}

void GLGE_C_FUNC(aabb_uniteToFirst)(AABB_t* first, const AABB_t* other) {((AABB*)first)->uniteToThis(*((AABB*)other));}

bool GLGE_C_FUNC(aabb_isInverted)(const AABB_t* aabb) {return ((AABB*)aabb)->isInverted();}

AABB_t* GLGE_C_FUNC(aabb_invert)(const AABB_t* aabb)
{
    //create the new AABB
    //for an explenation why it's a pointer see aabb_combine
    AABB* ret = new AABB();
    //set the AABB
    *ret = ((AABB*)aabb)->invert();
    //return the new AABB as a C type
    return (AABB_t*)ret;
}

void GLGE_C_FUNC(aabb_invertThis)(AABB_t* aabb) {((AABB*)aabb)->invertThis();}

bool GLGE_C_FUNC(aabb_contains)(const vec3* pos, AABB_t* aabb) {return ((AABB*)aabb)->contains(*pos);}