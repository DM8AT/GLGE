/**
 * @file GLGEVec2.h
 * @author DM8AT
 * @brief create the vector of 32 bit signed integers
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MATH_IVEC_
#define _GLGE_MATH_IVEC_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include the utilitys
#include "../../GLGECore/GLGEUtil.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE int32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME ivec2
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_ivec2

//inclue the vector template for 2D vectors
#include "Presets/GLGEVec2Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE int32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME ivec3
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_ivec3

//inclue the vector template for 3D vectors
#include "Presets/GLGEVec3Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE int32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME ivec4
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_ivec4

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
inline std::ostream& operator<<(std::ostream& os, const ivec2& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return int32_t the dot product of the two vectors
 */
inline int32_t dot(const ivec2& a, const ivec2& b) noexcept {return GLGE_C_FUNC(ivec2_dot)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const ivec3& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return int32_t the dot product of the two vectors
 */
inline int32_t dot(const ivec3& a, const ivec3& b) noexcept {return GLGE_C_FUNC(ivec3_dot)(a, b);}

/**
 * @brief compute the cross product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return the 3D cross product of the two vectors
 */
inline ivec3 cross(const ivec3& a, const ivec3& b) noexcept {return GLGE_C_FUNC(ivec3_cross)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const ivec4& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return int32_t the dot product of the two vectors
 */
inline int32_t dot(const ivec4& a, const ivec4& b) noexcept {return GLGE_C_FUNC(ivec4_dot)(a, b);}

#endif //C++ section

#endif