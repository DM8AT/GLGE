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

/**
 * @brief a simple utility function to use the seperating axis therom (SAT) to check if a triangle intersects with an AABB
 * 
 * @param a the first vertex of the triangle in the space of the AABB
 * @param b the second vertex of the triangle in the space of the AABB
 * @param c the thierd vertex of the triangle in the space of the AABB
 * @param extends the extends of the axis alligned bounding box (must be centerd at 0)
 * @param axis the axis to check for the collision
 * 
 * @return true : they intersect on this axis
 * @return false : they don't intersect on this axis
 */
static bool IntersectsTriangleAABB_SAT(const vec3& a, const vec3& b, const vec3& c, vec3 extends, vec3 axis)
{
    //compute the positions of the triangle's vertices 
    float pr0 = dot(a, axis);
    float pr1 = dot(b, axis);
    float pr2 = dot(c, axis);

    //project the aabb to the axis
    float prAABB = extends.x * fabs(dot(vec3(1,0,0), axis)) + 
                   extends.y * fabs(dot(vec3(0,1,0), axis)) + 
                   extends.z * fabs(dot(vec3(0,0,1), axis));
    
    //compute the minimum and maximum of the triangle's vertices on the axis
    float prMin = fminf(pr0, fminf(pr1, pr2));
    float prMax = fmaxf(pr0, fmaxf(pr1, pr2));

    //check if the AABB and triangle intersect
    return !(fmaxf(-prMax, prMin) > prAABB);
}

bool collide(const AABB& a, const Triangle& b) noexcept
{
    //calculate the center and extend of the AABB
    vec3 center = (a.getMin() + a.getMax()) / 2.;
    vec3 extend = a.getMax() - a.getMin();
    //compute the points relative to the AABB
    vec3 v0 = b.getA() - center;
    vec3 v1 = b.getB() - center;
    vec3 v2 = b.getC() - center;
    //calculate the unit direction vectors for the triangle's edges
    vec3 ab = (v1 - v0).normalize();
    vec3 bc = (v2 - v1).normalize();
    vec3 ca = (v0 - v2).normalize();

    //compute the axis perpandicular to the axis of the triangle and the normal of the AABB
    //Perpandicular to normal (1,0,0) (X-Axis)
    vec3 a00 = vec3(0, -ab.z, ab.y);
    vec3 a01 = vec3(0, -bc.z, bc.y);
    vec3 a02 = vec3(0, -ca.z, ca.y);
    //Perpandicular to normal (0,1,0) (Y-Axis)
    vec3 a10 = vec3(ab.z, 0, -ab.x);
    vec3 a11 = vec3(bc.z, 0, -bc.x);
    vec3 a12 = vec3(ca.z, 0, -ca.x);
    //Perpandicular to normal (0,0,1) (Z-Axis)
    vec3 a20 = vec3(-ab.y, ab.x, 0);
    vec3 a21 = vec3(-bc.y, bc.x, 0);
    vec3 a22 = vec3(-ca.y, ca.x, 0);

    //check if they intersect on each axis. If they don't intersect, instantly return as they can't intersect
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a00)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a01)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a02)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a10)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a11)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a12)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a20)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a21)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, a22)) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, vec3(1,0,0))) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, vec3(0,1,0))) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, vec3(0,0,1))) {return false;}
    if (!IntersectsTriangleAABB_SAT(v0, v1, v2, extend, cross(ab, bc))) {return false;}

    //if this point is reached, they intersect on each axis. So they intersect. 
    return true;
}

bool collide(const Sphere& a, const Triangle& b) noexcept
{
    //Algorithem source: https://realtimecollisiondetection.net/blog/?p=103
    //compute the vertices of the triangle in the space of the sphere
    vec3 v0 = b.getA() - a.getPos();
    vec3 v1 = b.getB() - a.getPos();
    vec3 v2 = b.getC() - a.getPos();
    //compute all edge vectors
    vec3 e01 = v1 - v0;
    vec3 e02 = v2 - v0;
    vec3 e10 = v0 - v1;
    vec3 e12 = v2 - v1;
    vec3 e20 = v0 - v2;
    vec3 e21 = v1 - v2;
    //cache the squared radius
    float r2 = a.getSquaredRadius();
    //compute the vector's normal
    vec3 triNormal = cross(e01, e02);
    //compute the distance between the sphere center and the plane formed by the triangle
    float d = fabs(dot(v0, triNormal));
    //compute a factor to correct for potentially nor normalized normal
    float e = dot(triNormal, triNormal);
    //they don't intersect if the distance is bigger than the radius. 
    bool sep1 = (d * d) > (r2 * e);

    //check if the vertices lie all on the outside
    //dot all vertices together to get all squared possibilities for intersection tests
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d02 = dot(v0, v2);
    float d11 = dot(v1, v1);
    float d12 = dot(v1, v2);
    float d22 = dot(v2, v2);
    //the sphere and triangle are seperated when all points lie on the outside of the sphere
    bool sep2 = ((d00 > r2) && (d01 > d00) && (d02 > d00)) ||
                ((d11 > r2) && (d01 > d11) && (d12 > d11)) ||
                ((d22 > r2) && (d02 > d22) && (d12 > d22));

    //check if the sphere intersects with a triangle's edge

    //V0 - V1 edge test
    //project scalings to project the point without division
    float d1 = dot(v0, e01);
    float e1 = dot(e01, e01);
    //calulate the projected point and make sure to correct the division
    vec3 Q1 = v0 * e1 - e01 * d1;
    vec3 QC = v2 * e1 - Q1;

    //V1 - V2 edge test
    //project scalings to project the point without division
    float d2 = dot(v1, e12);
    float e2 = dot(e12, e12);
    //calulate the projected point and make sure to correct the division
    vec3 Q2 = v1 * e2 - e12 * d2;
    vec3 QA = v0 * e2 - Q2;
    
    //V2 - V0 edge test
    //project scalings to project the point without division
    float d3 = dot(v2, e20);
    float e3 = dot(e20, e20);
    //calulate the projected point and make sure to correct the division
    vec3 Q3 = v2 * e3 - e20 * d3;
    vec3 QB = v1 * e3 - Q3;
    
    //finally, check if any of the axis overlap
    bool sep3 = ((dot(Q1, Q1) > (r2 * e1*e1)) && (dot(Q1, QC) > 0)) ||
                ((dot(Q2, Q2) > (r2 * e2*e2)) && (dot(Q2, QA) > 0)) || 
                ((dot(Q3, Q3) > (r2 * e3*e3)) && (dot(Q3, QB) > 0));

    //check for seperation and finaly return if they are not seperated
    return !(sep1 || sep2 || sep3);
}

bool collide(const Ray& a, const Triangle& b) noexcept
{
    #warning Collision Ray-Triangle not implemented
    return false;
}

bool collide(const Triangle& a, const Triangle& b) noexcept
{
    #warning Collision Triangle - Triangle not implemented
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

//AABB - Ray
bool collide_aabb_ray(const AABB_t* a, const Ray_t* b) {return collide(*((AABB*)a), *((Ray*)b));}

//Sphere - Ray
bool collide_aabb_ray(const Sphere_t* a, const Ray_t* b) {return collide(*((Sphere*)a), *((Ray*)b));}

//Ray - Ray
bool collide_aabb_ray(const Ray_t* a, const Ray_t* b) {return collide(*((Ray*)a), *((Ray*)b));}