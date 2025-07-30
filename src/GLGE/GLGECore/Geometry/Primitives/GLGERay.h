/**
 * @file GLGERay.h
 * @author DM8AT
 * @brief define how a ray is structured
 * @version 0.1
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_RAY_
#define _GLGE_RAY_

//include the settings
#include "../../GLGESettings.h"
//include utility functions for C function declarations
#include "../../GLGEUtil.h"
//include vectors
#include "../../../GLGEMath/Vectors/GLGEVectors.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a single ray
 */
class Ray
{
public:

    /**
     * @brief Construct a new Ray
     */
    Ray() = default;

    /**
     * @brief Construct a new Ray
     * 
     * @param origen the origen of the ray
     * @param dir the direction the ray is traveling in
     */
    inline Ray(const vec3& origen, const vec3& dir)
     : m_origen(origen), m_dir(dir.normalize()), m_idir(vec3(1)/m_dir) {}

    /**
     * @brief Get the position the ray is starting at
     * 
     * @return const vec3& the origen of the ray in 3D space
     */
    inline const vec3& getOrigen() const noexcept {return m_origen;}

    /**
     * @brief Get the direction the ray is traveling in
     * 
     * @return const vec3& the normalized vector pointing in the traveling direction
     */
    inline const vec3& getDir() const noexcept {return m_dir;}

    /**
     * @brief Get the reciprical of the ray's direction
     * 
     * @return const vec3& the reciprical of the ray's direction
     */
    inline const vec3& getRecipDir() const noexcept {return m_idir;}

    /**
     * @brief Set a new origen for the ray
     * 
     * @param origen the new position the ray is emitted from
     */
    inline void setOrigen(const vec3& origen) noexcept {m_origen = origen;}

    /**
     * @brief Set a new direction for the ray
     * 
     * @param dir the new direction the ray will travel in
     */
    inline void setDir(const vec3& dir) noexcept {m_dir = dir.normalize(); m_idir = vec3(1)/m_dir;}

    /**
     * @brief reflect this ray along a normal vector
     * 
     * @param normal the normal to reflect this ray along
     */
    inline void reflect(const vec3& normal) noexcept {m_dir = m_dir - normal*2*dot(normal, m_dir); m_idir = vec3(1.) / m_dir;}

    /**
     * @brief refract this ray along a normal vector
     * @warning this MAY set the direction to NaN,NaN,NaN if the angle is too step
     * 
     * @param normal the normal to reflect along
     * @param qr the quotient of the indices of refraction
     */
    void refract(const vec3& normal, float qr) noexcept;

    /**
     * @brief step the origen of the ray forward in some direction by a specific amount
     * 
     * @param length the amount to step forward by
     */
    inline void stepForward(float length) noexcept {m_origen += m_dir*length;}

    /**
     * @brief print the ray into an output stream
     * 
     * @param os the output stream to print to
     * @param r the ray to print
     * @return std::ostream& the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Ray& r) {return os << "Ray{origen: " << r.m_origen << ", dir: " << r.m_dir << "}";}

protected:

    /**
     * @brief store the point the ray is emitted from
     */
    vec3 m_origen;
    /**
     * @brief store the normalized direction the ray is traveling in
     */
    vec3 m_dir;
    /**
     * @brief store the reciprical of the ray direction (helps with some speed ups)
     */
    vec3 m_idir;

};

//start the C-Section
extern "C" {
#endif //end of C++ section

/**
 * @brief store an instance of the ray class
 */
typedef struct s_Ray Ray_t;

/**
 * @brief create a new instance of the ray class
 * 
 * @param origen the origen of the ray
 * @param dir the direction the ray is traveling in
 * @return Ray_t* a pointer to the new ray instance or 0 on an error
 */
Ray_t* GLGE_C_FUNC(ray_Create)(const vec3* origen, const vec3* dir);

/**
 * @brief delete an instance of the ray class
 * @warning this will invalidate all pointers to this instance
 * 
 * @param ray a pointer to the instance to destroy
 */
void GLGE_C_FUNC(ray_Destroy)(Ray_t* ray);

/**
 * @brief Get the position the ray is starting at
 * 
 * @param ray a pointer to the ray instance to querry the data from
 * 
 * @return const vec3* the origen of the ray in 3D space. The pointer will be valid as long as the ray instance is not moved nor destroyed. 
 */
const vec3* GLGE_C_FUNC(ray_getOrigen)(const Ray_t* ray);

/**
 * @brief Get the direction the ray is traveling in
 * 
 * @param ray a pointer to the ray instance to querry the data from
 * 
 * @return const vec3& the normalized vector pointing in the traveling direction
 */
const vec3* GLGE_C_FUNC(ray_getDir)(const Ray_t* ray);

/**
 * @brief Get the reciprical of the ray's direction
 * 
 * @param ray a pointer to the ray instance to querry the data from
 * 
 * @return const vec3& the reciprical of the ray's direction
 */
const vec3* GLGE_C_FUNC(ray_getRecpDir)(const Ray_t* ray);

/**
 * @brief Set a new origen for the ray
 * 
 * @param origen the new position the ray is emitted from
 * @param ray a pointer to the ray instance to update the origen of
 */
void GLGE_C_FUNC(ray_setOrigen)(const vec3* origen, Ray_t* ray);

/**
 * @brief Set a new direction for the ray
 * 
 * @param dir the new direction the ray will travel in
 * @param ray a pointer to the ray instance to update the direction of
 */
void GLGE_C_FUNC(ray_setDir)(const vec3* dir, Ray_t* ray);

/**
 * @brief reflect this ray along a normal vector
 * 
 * @param normal the normal to reflect this ray along
 * @param ray a pointer to the ray instance to reflect along the requested normal
 */
void GLGE_C_FUNC(ray_reflect)(const vec3* normal, Ray_t* ray);

/**
 * @brief refract this ray along a normal vector
 * @warning this MAY set the direction to NaN,NaN,NaN if the angle is too step
 * 
 * @param normal the normal to reflect along
 * @param qr the quotient of the indices of refraction
 * @param ray a pointer to the ray instance to refract with the given normal and quotient of refraction indices
 */
void GLGE_C_FUNC(ray_refract)(const vec3* normal, float qr, Ray_t* ray);

/**
 * @brief step the origen of the ray forward in some direction by a specific amount
 * 
 * @param length the amount to step forward by
 * @param ray a pointer to the ray instance to step forward in space
 */
void GLGE_C_FUNC(ray_stepForward)(float length, Ray_t* ray);

//check if a potential C-Section should end
#if GLGE_CPP
}
#endif //ended C-Section

#endif