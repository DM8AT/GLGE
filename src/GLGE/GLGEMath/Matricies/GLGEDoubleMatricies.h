/**
 * @file GLGEDoubleMatricies.h
 * @author DM8AT
 * @brief define all the double matrices
 * @version 0.1
 * @date 2025-07-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_DOUBLE_MATRICES_
#define _GLGE_DOUBLE_MATRICES_

//inlcude double vectors
#include "../Vectors/GLGEDoubleVec.h"

//define the double 2x2 matrix
#define GLGE_MAT_TYPE double
#define GLGE_MAT_NAME dmat2
#define GLGE_MAT_STRUCT_NAME s_dmat2
#define GLGE_MAT_VEC_TYPE dvec2
#include "Presets/GLGEMat2Preset.h"

//define the double 3x3 matrix
#define GLGE_MAT_TYPE double
#define GLGE_MAT_NAME dmat3
#define GLGE_MAT_STRUCT_NAME s_dmat3
#define GLGE_MAT_VEC_TYPE dvec3
#include "Presets/GLGEMat3Preset.h"

//define the double 4x4 matrix
#define GLGE_MAT_TYPE double
#define GLGE_MAT_NAME dmat4
#define GLGE_MAT_STRUCT_NAME s_dmat4
#define GLGE_MAT_VEC_TYPE dvec4
#include "Presets/GLGEMat4Preset.h"

#endif