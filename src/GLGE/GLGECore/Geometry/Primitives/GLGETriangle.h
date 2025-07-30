/**
 * @file GLGETriangle.h
 * @author DM8AT
 * @brief define what a triangle is
 * @version 0.1
 * @date 2025-06-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_TRIANGLE_
#define _GLGE_TRIANGLE_

//include the settings
#include "../../GLGESettings.h"
//include vectors
#include "../../../GLGEMath/Vectors/GLGEVectors.h"

//check for C++
#if GLGE_CPP

/**
 * @brief store a triangle that is created from 3 3D points
 */
class Triangle
{
public:

    /**
     * @brief Construct a new Triangle
     */
    Triangle() = default;

    /**
     * @brief Construct a new Triangle
     * 
     * @param _a the first point of the triangle
     * @param _b the second point of the triangle
     * @param _c the thired point of the triangle
     */
    Triangle(const vec3& _a, const vec3& _b, const vec3& _c)
     : a(_a), b(_b), c(_c) {}

    /**
     * @brief querry the position of the first vertex
     * 
     * @return const vec3& the first vertex
     */
    const vec3& getA() const noexcept {return a;}

    /**
     * @brief querry the position of the second vertex
     * 
     * @return const vec3& the second vertex
     */
    const vec3& getB() const noexcept {return b;}

    /**
     * @brief querry the position of the thierd vertex
     * 
     * @return const vec3& the thierd vertex
     */
    const vec3& getC() const noexcept {return c;}

    /**
     * @brief set the position of the first vertex
     * 
     * @param _a the new position for the first vertex
     */
    void setA(const vec3& _a) noexcept {a = _a;}

    /**
     * @brief set the position of the second vertex
     * 
     * @param _a the new position for the second vertex
     */
    void setB(const vec3& _b) noexcept {b = _b;}

    /**
     * @brief set the position of the thierd vertex
     * 
     * @param _a the new position for the thierd vertex
     */
    void setC(const vec3& _c) noexcept {c = _c;}

    /**
     * @brief Get the area of the triangle
     * 
     * @return float the area of the triangle
     */
    float getArea() const noexcept;

    /**
     * @brief move the whole triangle in 3D space
     * 
     * @param delta the amount to move the triangle by
     */
    void translate(const vec3& delta) noexcept;

    /**
     * @brief scale the triangle
     * 
     * @param scale the amount per axis to scale
     */
    void scale(const vec3& scale) noexcept;

    /**
     * @brief Get the Weights for interpolation along the triangle
     * The weights are normalized to sum up to 1
     * 
     * @param pos the position to calculate the interpolation weights for
     * @return vec3 the weights for all vertices. X corresponds to A, Y corresponds to B and Z corresponds to C
     */
    vec3 getWeights(const vec3& pos) const noexcept;

    /**
     * @brief interpolate some value across the triangle
     * @warning if the position to interpolate for lies outside of the triangle, it will be exterpolated
     * 
     * @tparam T the type of the value to interpolate over the triangle
     * @param a_t the value at the vertex a
     * @param b_t the value at the vertex b
     * @param c_t the value at the vertex c
     * @param pos the position to interpolate for
     * @param inside a pointer to a bool to store if the point is insde the triangle. When 0 is inputed, it won't be stored. 
     * @return T the value of the inputs interpolated over the triangle
     */
    template <typename T> T interpolate(const T& a_t, const T& b_t, const T& c_t, const vec3& pos, bool* inside) const noexcept
    {
        //get the weights for all axis
        vec3 weights = getWeights(pos);
        //check and store if the point is inside the triangle
        //the point is inside the triangle when all sub-areas are in the range [0 ; 1]
        //they all sum to 1, so if one is less than 0 than the other ones are others must be out of range
        //but only store the weights if the pointer is not 0
        if (inside) {*inside = (0. <= weights.x) && (0. <= weights.y) && (0. <= weights.z);}

        //mix the inputed values based on the calculated weights
        return (a_t * weights.x) + (b_t * weights.y) + (c_t * weights.z);
    }

protected:

    /**
     * @brief store all 3 points of the triangle
     */
    vec3 a, b, c;

};

//start a C section
extern "C" {
#endif //end of C++ section

/**
 * @brief store an instance of the triangle class
 */
typedef struct s_Triangle Triangle_t;

/**
 * @brief create a new instance of the triangle class
 * 
 * @param a the first point of the triangle
 * @param b the second point of the triangle
 * @param c the thired point of the triangle
 * @return Triangle_t* a pointer to the new triangle instance
 */
Triangle_t* GLGE_C_FUNC(triangle_Create)(const vec3* a, const vec3* b, const vec3* c);

/**
 * @brief destroy an instance of the triangle instance
 * @warning this will invalidte all points to this instance
 * 
 * @param triangle a pointer to the triangle instance to delete
 */
void GLGE_C_FUNC(triangle_Destroy)(Triangle_t* triangle);

/**
 * @brief querry the position of the first vertex
 * 
 * @param triangle a pointer to the triangle to querry the data from
 * 
 * @return const vec3& the first vertex
 */
const vec3* GLGE_C_FUNC(triangle_getA)(const Triangle_t* triangle);

/**
 * @brief querry the position of the second vertex
 * 
 * @param triangle a pointer to the triangle to querry the data from
 * 
 * @return const vec3& the second vertex
 */
const vec3* GLGE_C_FUNC(triangle_getB)(const Triangle_t* triangle);

/**
 * @brief querry the position of the thierd vertex
 * 
 * @param triangle a pointer to the triangle to querry the data from
 * 
 * @return const vec3& the thierd vertex
 */
const vec3* GLGE_C_FUNC(triangle_getC)(const Triangle_t* triangle);

/**
 * @brief set the position of the first vertex
 * 
 * @param val the new position for the first vertex
 * @param a pointer to the triangle instance to update the vertex of
 */
void GLGE_C_FUNC(triangle_setA)(const vec3* val, Triangle_t* triangle);

/**
 * @brief set the position of the second vertex
 * 
 * @param val the new position for the second vertex
 * @param a pointer to the triangle instance to update the vertex of
 */
void GLGE_C_FUNC(triangle_setB)(const vec3* val, Triangle_t* triangle);

/**
 * @brief set the position of the thierd vertex
 * 
 * @param val the new position for the thierd vertex
 * @param a pointer to the triangle instance to update the vertex of
 */
void GLGE_C_FUNC(triangle_setC)(const vec3* val, Triangle_t* triangle);

/**
 * @brief Get the area of the triangle
 * 
 * @param triangle a pointer to the triangle to calculate the area of
 * 
 * @return float the area of the triangle
 */
float GLGE_C_FUNC(triangle_getArea)(const Triangle_t* triangle);

/**
 * @brief move the whole triangle in 3D space
 * 
 * @param delta the amount to move the triangle by
 * @param triangle a pointer the triangle instance to translate
 */
void GLGE_C_FUNC(triangle_translate)(const vec3* delta, Triangle_t* triangle);

/**
 * @brief scale the triangle
 * 
 * @param scale the amount per axis to scale
 * @param triangle a pointer to the triangle instance to scale
 */
void GLGE_C_FUNC(triangle_scale)(const vec3* scale, Triangle_t* triangle);

/**
 * @brief Get the Weights for interpolation along the triangle
 * The weights are normalized to sum up to 1
 * 
 * @param pos the position to calculate the interpolation weights for
 * @param triangle a pointer to the triangle to querry the weights from
 * @return vec3 the weights for all vertices. X corresponds to A, Y corresponds to B and Z corresponds to C
 */
vec3 GLGE_C_FUNC(triangle_getWeights)(const vec3* pos, const Triangle_t* triangle);

//end the C section if it exists
#if GLGE_CPP
}
#endif //end of C section

#endif