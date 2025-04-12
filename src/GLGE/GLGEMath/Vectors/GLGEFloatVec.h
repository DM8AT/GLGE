/**
 * @file GLGEVec2.h
 * @author DM8AT
 * @brief create the vectors of floats
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MATH_VEC_
#define _GLGE_MATH_VEC_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include the utilitys
#include "../../GLGECore/GLGEUtil.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE float
//define the name of the next vector to create
#define GLGE_VEC_NAME vec2
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_vec2

//inclue the vector template for 2D vectors
#include "Presets/GLGEVec2Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE float
//define the name of the next vector to create
#define GLGE_VEC_NAME vec3
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_vec3

//inclue the vector template for 3D vectors
#include "Presets/GLGEVec3Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE float
//define the name of the next vector to create
#define GLGE_VEC_NAME vec4
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_vec4

//inclue the vector template for 4D vectors
#include "Presets/GLGEVec4Preset.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const vec2& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return float the dot product of the two vectors
 */
inline float dot(const vec2& a, const vec2& b) noexcept {return GLGE_C_FUNC(vec2_dot)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const vec3& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return float the dot product of the two vectors
 */
inline float dot(const vec3& a, const vec3& b) noexcept {return GLGE_C_FUNC(vec3_dot)(a, b);}

/**
 * @brief compute the cross product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return the 3D cross product of the two vectors
 */
inline vec3 cross(const vec3& a, const vec3& b) noexcept {return GLGE_C_FUNC(vec3_cross)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const vec4& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return float the dot product of the two vectors
 */
inline float dot(const vec4& a, const vec4& b) noexcept {return GLGE_C_FUNC(vec4_dot)(a, b);}

#endif //C++ section

#endif