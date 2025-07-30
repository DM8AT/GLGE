/**
 * @file GLGEIntMatricies.h
 * @author DM8AT
 * @brief define all the integer matrices
 * @version 0.1
 * @date 2025-07-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_INT_MATRICES_
#define _GLGE_INT_MATRICES_

//inlcude integer vectors
#include "../Vectors/GLGEIntVec.h"

//define the integer 2x2 matrix
#define GLGE_MAT_TYPE int32_t
#define GLGE_MAT_NAME imat2
#define GLGE_MAT_STRUCT_NAME s_imat2
#define GLGE_MAT_VEC_TYPE ivec2
#include "Presets/GLGEMat2Preset.h"

//define the integer 3x3 matrix
#define GLGE_MAT_TYPE int32_t
#define GLGE_MAT_NAME imat3
#define GLGE_MAT_STRUCT_NAME s_imat3
#define GLGE_MAT_VEC_TYPE ivec3
#include "Presets/GLGEMat3Preset.h"

//define the int32_t 4x4 matrix
#define GLGE_MAT_TYPE int32_t
#define GLGE_MAT_NAME imat4
#define GLGE_MAT_STRUCT_NAME s_imat4
#define GLGE_MAT_VEC_TYPE ivec4
#include "Presets/GLGEMat4Preset.h"

#endif