/**
 * @file GLGESphere.h
 * @author DM8AT
 * @brief define what a sphere is in GLGE
 * @version 0.1
 * @date 2025-06-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_SPHERE_
#define _GLGE_SPHERE_

//include the settings
#include "../../GLGESettings.h"
//include utilites for C function definitions
#include "../../GLGEUtil.h"
//include all vector types
#include "../../../GLGEMath/Vectors/GLGEVectors.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a simple sphere and utility functions to maniuplate the sphere
 */
class Sphere
{
public:

    /**
     * @brief Construct a new Sphere
     */
    Sphere() = default;

    /**
     * @brief Construct a sphere that includes all specified positions
     * 
     * @param positions a list of positions
     * @param posCount the amount of positions in the list
     */
    Sphere(const vec3* positions, uint64_t posCount);

    /**
     * @brief Construct a new Sphere
     * 
     * @param pos the position of the sphere
     * @param radius the radius of the sphere
     */
    Sphere(const vec3& pos, float radius)
     : m_pos(pos), m_radius(radius) {}

    /**
     * @brief Get the position of the sphere's center
     * 
     * @return const vec3& the position of the sphere
     */
    inline const vec3& getPos() const noexcept {return m_pos;}

    /**
     * @brief Get the radius of the sphere
     * 
     * @return const float& the radius of the sphere
     */
    inline const float& getRadius() const noexcept {return m_radius;}

    /**
     * @brief Set a new position for the sphere
     * 
     * @param pos the new position for the sphere
     */
    inline void setPos(const vec3& pos) noexcept {m_pos = pos;}

    /**
     * @brief Set the radius of the sphere
     * 
     * @param radius the new radius of the sphere
     */
    inline void setRadius(float radius) noexcept {m_radius = radius;}

    /**
     * @brief Get the squared radius of the sphere
     * 
     * @return const float& the squared radius of the sphere used for fast collision checks
     */
    inline float getSquaredRadius() const noexcept {return m_radius*m_radius;}

    /**
     * @brief move the sphere in 3D space
     * 
     * @param delta the amount to move the sphere by
     */
    inline void translate(vec3 delta) noexcept {m_pos += delta;}

    /**
     * @brief scale the sphere by some amount
     * 
     * @param scale the amount to scale the sphere by
     */
    inline void scale(float scale) noexcept {m_radius *= scale;}

    /**
     * @brief scale the sphere by some amount
     * 
     * This is mostly for compatability. Only the largest axis is used for scaling to ensure it reaches at least the requested size
     * 
     * @param _scale the 3D vector to scale with. ONLY THE LARGEST AXIS IS USED!
     */
    inline void scale(vec3 _scale) noexcept {scale(((_scale.x > _scale.y) && (_scale.x > _scale.z)) ? _scale.x : ((_scale.y > _scale.z) ? _scale.y : _scale.z));}

    /**
     * @brief Get the squared distance between the inputed point and the sphere's surface
     * 
     * @param point the point to get the distance to
     * @return float the squared distance between the AABB and the point
     */
    float getSqrDistanceFrom(const vec3& point) const noexcept;

    /**
     * @brief Get the euclidian distance between the sphere and the inputed point
     * @warning This uses the square root operation. Just for comparisons it's faster to use the squared distance
     * 
     * @param point the point to calculate the distance to
     * @return float the euclidian distance between the sphere and the inputed point
     */
    inline float getDistanceFrom(const vec3& point) const noexcept {return sqrt(getSqrDistanceFrom(point));};

    /**
     * @brief print the sphere into an output stream
     * 
     * @param os the output stream to print to
     * @param s the sphere to print
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Sphere& s) noexcept 
    {return os << "Sphere{pos: " << s.m_pos << ", radius: " << s.m_radius << "}";}

protected:

    /**
     * @brief store the position of the sphere
     */
    vec3 m_pos;
    /**
     * @brief store the radius of the sphere
     */
    float m_radius = 0;

};

//start a section for C
extern "C" {
#endif //C++ section

/**
 * @brief store an instance of the Sphere class
 */
typedef struct s_Sphere Sphere_t;

/**
 * @brief create a new instance of the sphere class
 * 
 * @param pos the position for the sphere
 * @param radius the radius of the sphere
 * @return Sphere_t* a pointer to the new sphere object
 */
Sphere_t* GLGE_C_FUNC(sphere_Create)(const vec3* pos, float radius);

/**
 * @brief create a new sphere instance from a point cloude
 * 
 * @param points a list of positions representing the point cloude
 * @param pointCount the amount of points in the point cloude
 * @return Sphere_t* the new sphere instance containing all points from the point cloude
 */
Sphere_t* GLGE_C_FUNC(sphere_FromPoints)(const vec3* points, uint64_t pointCount);

/**
 * @brief delete an instance of the sphere class
 * @warning this will invalidate all pointers to this object
 * 
 * @param sphere a pointer to the sphere object to destroy
 */
void GLGE_C_FUNC(sphere_Destroy)(Sphere_t* sphere);

/**
 * @brief querry the position of a sphere instance
 * 
 * @param sphere a pointer to the sphere instance to querry the position data from
 * @return const vec3* a constant pointer to the sphere's position that remains valid as long as the object exists
 */
const vec3* GLGE_C_FUNC(sphere_getPos)(const Sphere_t* sphere);

/**
 * @brief qerry the radius of a sphere instance
 * 
 * @param sphere a pointer to the sphere instance to querry the radius data from
 * @return float the radius of the sphere
 */
float GLGE_C_FUNC(sphere_getRadius)(const Sphere_t* sphere);

/**
 * @brief change the position of a sphere instance
 * 
 * @param pos the new position for the sphere instance
 * @param sphere a pointer to the sphere instance to change the position of
 */
void GLGE_C_FUNC(sphere_setPos)(const vec3* pos, Sphere_t* sphere);

/**
 * @brief change the radius of a sphere instance
 * 
 * @param radius the new radius for the sphere instance
 * @param sphere a pointer to the sphere instance to change the radius of
 */
void GLGE_C_FUNC(sphere_setRadius)(float radius, Sphere_t* sphere);

/**
 * @brief get the squared radius of the sphere
 * 
 * @param sphere a pointer to the sphere instance to querry the data from
 * @return float the squared radius of the sphere
 */
float GLGE_C_FUNC(sphere_getSquaredRadius)(const Sphere_t* sphere);

/**
 * @brief move a sphere instance in 3D space
 * 
 * @param delta the amount to change the position by
 * @param sphere a pointer to the sphere instance to move
 */
void GLGE_C_FUNC(sphere_translate)(const vec3* delta, Sphere_t* sphere);

/**
 * @brief scale a sphere instance uniformly on all axis
 * 
 * @param scale the amount to scale the sphere instance by
 * @param sphere a pointer to the sphere instance to scale
 */
void GLGE_C_FUNC(sphere_scaleUniform)(float scale, Sphere_t* sphere);

/**
 * @brief scale a sphere instance
 * @warning only the longest axis will be used for scaling to ensure all the space is included
 * 
 * @param scale a vector to scale by
 * @param sphere a pointer to the sphere instance to scale
 */
void GLGE_C_FUNC(sphere_scale)(const vec3* scale, Sphere_t* sphere);

/**
 * @brief calculate the squared distance between a point and a sphere instance
 * 
 * @param point the point to calculate the distance from
 * @param sphere a pointer to the sphere instance to calcualte the distance to
 * @return float the squared distance between the point and sphere instance
 */
float GLGE_C_FUNC(sphere_getSqrDistanceFrom)(const vec3* point, const Sphere_t* sphere);

/**
 * @brief calculate the distance between a point and the sphere
 * 
 * @param point the point to calculate the distance from
 * @param sphere a pointer to the sphere instance to calculate the distance to
 * @return float the distance between the point and sphere instance
 */
float GLGE_C_FUNC(sphere_getDistanceFrom)(const vec3* point, const Sphere_t* sphere);

//check if a potential C-Section should end
#if GLGE_CPP
}
#endif // End of C-Section

#endif