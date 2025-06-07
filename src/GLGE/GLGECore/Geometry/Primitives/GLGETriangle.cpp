/**
 * @file GLGETriangle.cpp
 * @author DM8AT
 * @brief implement all larget not-template functions for the triangle
 * @version 0.1
 * @date 2025-06-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the triangle
#include "GLGETriangle.h"

void Triangle::translate(const vec3& delta) noexcept
{
    //change all positions by the delta
    a += delta;
    b += delta;
    c += delta;
}

void Triangle::scale(const vec3& scale) noexcept
{
    //multiply all points by the scale
    a *= scale;
    b *= scale;
    c *= scale;
}

float Triangle::getArea() const noexcept
{
    //compute the vectors for the sides
    vec3 ab = b - a;
    vec3 ac = c - a;

    //pre-compute all components for squaring later
    float _a = ab.y * ac.z - ab.z * ac.y;
    float _b = ab.z * ac.x - ab.x * ac.z;
    float _c = ab.x * ac.y - ab.y * ac.x;

    //return the area
    return 0.5 * sqrt((_a*_a) + (_b*_b) + (_c*_c));
}

vec3 Triangle::getWeights(const vec3& pos) const noexcept
{
    //compute the reciprical of the whole triangle's are
    float ra = 1. / getArea();
    //compute the area of 3 sub-triangles
    //the first two are points of the triangle and the last one is the test point
    //divide all areas by the whole area (multiply by reciprical) to normalize the area's sum to 1 
    float abp = Triangle(a, b, pos).getArea() * ra;
    float cap = Triangle(c, a, pos).getArea() * ra;
    //the last area can be calculated by subtracting both other areas from 1
    float bcp = 1. - (abp + cap);

    //return the weights as a 3D vector
    return vec3(abp, bcp, cap);
}

//C-Function maps

//A function map is as follows: Cast to the C++ type and then execute the desired functionallity
//if a sphere object is returned, cast it to the C type before returning it. 

Triangle_t* GLGE_C_FUNC(triangle_Create)(const vec3* a, const vec3* b, const vec3* c) {return (Triangle_t*)(new Triangle(*a, *b, *c));}

void GLGE_C_FUNC(triangle_Destroy)(Triangle_t* triangle) {delete (Triangle*)triangle;}

const vec3* GLGE_C_FUNC(triangle_getA)(const Triangle_t* triangle) {return &((Triangle*)triangle)->getA();}

const vec3* GLGE_C_FUNC(triangle_getB)(const Triangle_t* triangle) {return &((Triangle*)triangle)->getB();}

const vec3* GLGE_C_FUNC(triangle_getC)(const Triangle_t* triangle) {return &((Triangle*)triangle)->getC();}

void GLGE_C_FUNC(triangle_setA)(const vec3* val, Triangle_t* triangle) {((Triangle*)triangle)->setA(*val);}

void GLGE_C_FUNC(triangle_setB)(const vec3* val, Triangle_t* triangle) {((Triangle*)triangle)->setB(*val);}

void GLGE_C_FUNC(triangle_setC)(const vec3* val, Triangle_t* triangle) {((Triangle*)triangle)->setC(*val);}

float GLGE_C_FUNC(triangle_getArea)(const Triangle_t* triangle) {((Triangle*)triangle)->getArea();}

void GLGE_C_FUNC(triangle_translate)(const vec3* delta, Triangle_t* triangle) {((Triangle*)triangle)->translate(*delta);}

void GLGE_C_FUNC(triangle_scale)(const vec3* scale, Triangle_t* triangle) {((Triangle*)triangle)->scale(*scale);}

vec3 GLGE_C_FUNC(triangle_getWeights)(const vec3* pos, const Triangle_t* triangle) {return ((Triangle*)triangle)->getWeights(*pos);}
