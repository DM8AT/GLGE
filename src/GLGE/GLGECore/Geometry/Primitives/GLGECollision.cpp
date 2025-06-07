/**
 * @file GLGECollision.cpp
 * @author DM8AT
 * @brief implement the functions to check for collisions between all primitive types
 * @version 0.1
 * @date 2025-06-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the collision header
#include "GLGECollision.h"

//AABB - AABB
bool collide(const AABB& a, const AABB& b) noexcept
{
    //check if the minima and maxima are in the correct order for intersection
    return (
        (a.m_min.x <= b.m_max.x) &&
        (a.m_max.x >= b.m_min.x) &&
        (a.m_min.y <= b.m_max.y) &&
        (a.m_max.y >= b.m_min.y) &&
        (a.m_min.z <= b.m_max.z) &&
        (a.m_max.z >= b.m_min.z)
    );
}

//AABB - Sphere
bool collide(const AABB& a, const Sphere& b) noexcept
{
    //compute the squared distance between the AABB and the Sphere's center
    float sqrDist = a.getSqrDistanceFrom(b.getPos());
    //if the squared distance is less or equal to the sphere's radius, they intersect
    return (sqrDist <= b.getSquaredRadius());
}

//Sphere - Sphere
bool collide(const Sphere& a, const Sphere& b) noexcept
{
    //compute the distance between the sphere's centers
    float len = (a.getPos() - b.getPos()).length();
    //if the length is less than the sum of the radia, they overlap
    return (len < (a.getRadius() + b.getRadius()));
}


bool collide(const AABB& a, const Ray& b) noexcept
{
    //compute the near and far intersections
    vec3 near = (a.getMin() - b.getOrigen()) * b.getRecipDir();
    vec3 far = (a.getMax() - b.getOrigen()) * b.getRecipDir();

    //correct the minimums and maximums
    if (near.x > far.x) {std::swap(near.x, far.x);}
    if (near.y > far.y) {std::swap(near.y, far.y);}
    if (near.z > far.z) {std::swap(near.z, far.z);}

    //check if the ray misses the bounding box
    //that happens when the ray exits it before it enters
    if (
        (near.x > far.y) || (near.x > far.z) || 
        (near.y > far.x) || (near.y > far.z) ||
        (near.z > far.x) || (near.z > far.y)
    ) {return false;}

    //get the exit time
    float far_hit = fminf(fminf(far.x, far.y), far.z);

    //check if the last exit point is behind the origen
    //if that is the case, it would be less than 0
    //in that case, the ray also misses the box
    if (far_hit < 0) {return false;}

    //finaly, if this point is reached, the ray hits the box
    return true;
}

bool collide(const Sphere& sphere, const Ray& aabb) noexcept
{
    //compute the coefficents of the quadratic equation
    float a = dot(aabb.getDir(), aabb.getDir());
    float b = 2. * dot(aabb.getDir(), aabb.getOrigen());
    float c = dot(aabb.getOrigen(), aabb.getOrigen()) - sphere.getSquaredRadius();

    //pre - calculate the discriminant
    float discr = (b*b) - (4. * a*c);

    //check if the discriminant is less than 0. Then, there is no intersection. 
    if (discr < 0) {return false;}

    //pre-calculate reciprical
    float recp = 1. / (2. * a);
    //calcualte the square root
    float discr_sqrt = sqrt(discr);
    //calculate t0 and t1
    //float t0 = (-b - discr_sqrt) * recp; //t0 is never used
    float t1 = (discr_sqrt - b) * recp;

    //if t1 is less than 0, the ray exited the sphere behind the ray's origen
    //this means that there is no intersection. 
    //so an intersection occoured if t1 is greater or equal to 0. 
    return (t1 >= 0);
}

bool collide(const Ray& a, const Ray& b) noexcept
{
    #warning Ray - Ray intersection not implemented
    return false;
}







//C-Functions

//function mappings just cast and de-reference both inputs to call the C++ funciton on them

//AABB - AABB
bool collide(const AABB_t* a, const AABB_t* b) {return collide(*((AABB*)a), *((AABB*)b));}

//AABB - Sphere
bool collide_aabb_sphere(const AABB_t* a, const Sphere_t* b) {return collide(*((AABB*)a), *((Sphere*)b));}

//Sphere - Sphere
bool collide_sphere_sphere(const Sphere_t* a, const Sphere_t* b) {return collide(*((Sphere*)a), *((Sphere*)b));}