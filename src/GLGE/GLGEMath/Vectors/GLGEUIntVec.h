/**
 * @file GLGEVec2.h
 * @author DM8AT
 * @brief create the vector of 32 bit unsigned integers
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MATH_UVEC_
#define _GLGE_MATH_UVEC_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include the utilitys
#include "../../GLGECore/GLGEUtil.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE uint32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME uvec2
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_uvec2

//inclue the vector template for 2D vectors
#include "Presets/GLGEVec2Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE uint32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME uvec3
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_uvec3

//inclue the vector template for 3D vectors
#include "Presets/GLGEVec3Preset.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE uint32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME uvec4
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_uvec4

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
inline std::ostream& operator<<(std::ostream& os, const uvec2& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return uint32_t the dot product of the two vectors
 */
inline uint32_t GLGE_C_FUNC(dot)(const uvec2& a, const uvec2& b) noexcept {return GLGE_C_FUNC(uvec2_dot)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const uvec3& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return uint32_t the dot product of the two vectors
 */
inline uint32_t GLGE_C_FUNC(dot)(const uvec3& a, const uvec3& b) noexcept {return GLGE_C_FUNC(uvec3_dot)(a, b);}

/**
 * @brief compute the cross product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return the 3D cross product of the two vectors
 */
inline uvec3 GLGE_C_FUNC(cross)(const uvec3& a, const uvec3& b) noexcept {return GLGE_C_FUNC(uvec3_cross)(a, b);}

/**
 * @brief print the vector into the default output
 * 
 * @param os the output stream to print into
 * @param vec the vector to print
 * @return std::ostream& a reference to the stream with the vector content
 */
inline std::ostream& operator<<(std::ostream& os, const uvec4& vec) noexcept {return os << vec.asString();}

/**
 * @brief compute the dot product of two vectors
 * 
 * @param a the first vector
 * @param b the second vector
 * @return uint32_t the dot product of the two vectors
 */
inline uint32_t GLGE_C_FUNC(dot)(const uvec4& a, const uvec4& b) noexcept {return GLGE_C_FUNC(uvec4_dot)(a, b);}

#endif //C++ section

#endif