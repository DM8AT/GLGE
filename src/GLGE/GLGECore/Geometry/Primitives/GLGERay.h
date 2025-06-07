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

//check if a potential C-Section should end
#if GLGE_CPP
}
#endif //ended C-Section

#endif