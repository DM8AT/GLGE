/**
 * @file GLGEFloatMatricies.h
 * @author DM8AT
 * @brief define all the float matrices
 * @version 0.1
 * @date 2025-07-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_FLOAT_MATRICES_
#define _GLGE_FLOAT_MATRICES_

//inlcude float vectors
#include "../Vectors/GLGEFloatVec.h"

//define the float 2x2 matrix
#define GLGE_MAT_TYPE float
#define GLGE_MAT_NAME mat2
#define GLGE_MAT_STRUCT_NAME s_mat2
#define GLGE_MAT_VEC_TYPE vec2
#include "Presets/GLGEMat2Preset.h"

//define the float 3x3 matrix
#define GLGE_MAT_TYPE float
#define GLGE_MAT_NAME mat3
#define GLGE_MAT_STRUCT_NAME s_mat3
#define GLGE_MAT_VEC_TYPE vec3
#include "Presets/GLGEMat3Preset.h"

//define the float 4x4 matrix
#define GLGE_MAT_TYPE float
#define GLGE_MAT_NAME mat4
#define GLGE_MAT_STRUCT_NAME s_mat4
#define GLGE_MAT_VEC_TYPE vec4
#include "Presets/GLGEMat4Preset.h"

#endif