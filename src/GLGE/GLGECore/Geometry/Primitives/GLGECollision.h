/**
 * @file GLGECollision.h
 * @author DM8AT
 * @brief store collision functions between all primitive types
 * @version 0.1
 * @date 2025-06-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_COLLISION_
#define _GLGE_COLLISION_

//include AABBs
#include "GLGE_AABB.h"
//include spheres
#include "GLGESphere.h"
//include rays
#include "GLGERay.h"

//check for C++
#if GLGE_CPP

/**
 * @brief check for a collision between two axis alligned bounding boxes
 * 
 * @param a the first bounding box to check 
 * @param b the second bounding box to check 
 * @return true : the boxes overlap somewhere
 * @return false : the boxes don't collide
 */
bool collide(const AABB& a, const AABB& b) noexcept;

/**
 * @brief check for a collision between an axis alligned bounding box and a sphere
 * 
 * @param a the axis alligned bounding box for collision
 * @param b the sphere for collision
 * @return true : both objects intersect somewhere
 * @return false : the objects do not intersect
 */
bool collide(const AABB& a, const Sphere& b) noexcept;

/**
 * @brief check for a collision between a sphere and an axis alligned bounding box
 * 
 * @param a the sphere for collision
 * @param b the axis alligned bounding box for collision
 * @return true : both objects intersect somewhere
 * @return false : the objects do not intersect
 */
inline bool collide(const Sphere& a, const AABB& b) noexcept {return collide(b, a);}

/**
 * @brief check for a collision between two spheres
 * 
 * @param a one of the spheres to check for collision
 * @param b the other sphere to check for collision
 * @return true : both spheres intersect somewhere
 * @return false : the spheres do not intersect
 */
bool collide(const Sphere& a, const Sphere& b) noexcept;

bool collide(const AABB& a, const Ray& b) noexcept;

inline bool collide(const Ray& a, const AABB& b) noexcept {return collide(b,a);}

bool collide(const Sphere& a, const Ray& b) noexcept;

inline bool collide(const Ray& a, const Sphere& b) noexcept {return collide(b,a);}

bool collide(const Ray& a, const Ray& b) noexcept;

//start a C-Section
extern "C" {
#endif //end of C++ section

/**
 * @brief check for a collision between two axis alligned bounding boxes
 * 
 * @param a the first bounding box to check 
 * @param b the second bounding box to check 
 * @return true : the boxes overlap somewhere
 * @return false : the boxes don't collide
 */
bool collide_aabb_aabb(const AABB_t* a, const AABB_t* b);

/**
 * @brief check for a collision between an axis alligned bounding box and a sphere
 * 
 * @param a the bounding box to check for collision
 * @param b the sphere to check for collision
 * @return true : the objects overlap somewhere
 * @return false : the objects don't overlap
 */
bool collide_aabb_sphere(const AABB_t* a, const Sphere_t* b);

/**
 * @brief check for a collision between a sphere and an axis alligned bounding box
 * 
 * @param a the sphere to check for collision
 * @param b the axis alligned bounding box to check for collision
 * @return true : the objects overlap somewhere
 * @return false : the objects don't overlap
 */
inline bool collide_sphere_aabb(const Sphere_t* a, const AABB_t* b) {return collide_aabb_sphere(b, a);}

/**
 * @brief check for a collision between two spheres
 * 
 * @param a one of the spheres to check for collision
 * @param b the other sphere to check for collision
 * @return true : the spheres overlap somewhere
 * @return false : the spheres do not overlap
 */
bool collide_sphere_sphere(const Sphere_t* a, const Sphere_t* b);

//check for a potential C-Section to end
#if GLGE_CPP
}
#endif //end of C-Section

#endif