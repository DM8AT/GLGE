/**
 * @file GLGEUIntMatricies.h
 * @author DM8AT
 * @brief define all the unsigned integer matrices
 * @version 0.1
 * @date 2025-07-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_UINT_MATRICES_
#define _GLGE_UINT_MATRICES_

//inlcude unsigned vector
#include "../Vectors/GLGEUIntVec.h"

//define the unsigned integer 2x2 matrix
#define GLGE_MAT_TYPE uint32_t
#define GLGE_MAT_NAME umat2
#define GLGE_MAT_STRUCT_NAME s_umat2
#define GLGE_MAT_VEC_TYPE uvec2
#include "Presets/GLGEMat2Preset.h"

#endif