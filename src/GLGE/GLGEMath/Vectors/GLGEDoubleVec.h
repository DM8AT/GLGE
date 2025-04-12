/**
 * @file GLGEVec2.h
 * @author DM8AT
 * @brief create the vectors of doubles
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MATH_DVEC_
#define _GLGE_MATH_DVEC_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include the utilitys
#include "../../GLGECore/GLGEUtil.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE double
//define the name of the next vector to create
#define GLGE_VEC_NAME dvec2
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_dvec2

//inclue the vector template for 2D vectors
#include "Presets/GLGEVec2Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE double
//define the name of the next vector to create
#define GLGE_VEC_NAME dvec3
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_dvec3

//inclue the vector template for 3D vectors
#include "Presets/GLGEVec3Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE double
//define the name of the next vector to create
#define GLGE_VEC_NAME dvec4
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_dvec4

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
inline std::ostream& operator<<(std::ostream& os, const dvec2& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return double the dot product of the two vectors
 */
inline double GLGE_C_FUNC(dot)(const dvec2& a, const dvec2& b) noexcept {return GLGE_C_FUNC(dvec2_dot)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const dvec3& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return double the dot product of the two vectors
 */
inline double GLGE_C_FUNC(dot)(const dvec3& a, const dvec3& b) noexcept {return GLGE_C_FUNC(dvec3_dot)(a, b);}

/**
 * @brief compute the cross product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return the 3D cross product of the two vectors
 */
inline dvec3 GLGE_C_FUNC(cross)(const dvec3& a, const dvec3& b) noexcept {return GLGE_C_FUNC(dvec3_cross)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const dvec4& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return double the dot product of the two vectors
 */
inline double GLGE_C_FUNC(dot)(const dvec4& a, const dvec4& b) noexcept {return GLGE_C_FUNC(dvec4_dot)(a, b);}

#endif //C++ section

#endif