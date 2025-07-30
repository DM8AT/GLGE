/**
 * @file GLGEUIntMatricies.cpp
 * @author DM8AT
 * @brief implement the unsigned integer matricies
 * @version 0.1
 * @date 2025-07-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the unsigned integer matricies
#include "GLGEUIntMatricies.h"

//implement the unsigned integer 2x2 matrix
#define GLGE_MAT_TYPE uint32_t
#define GLGE_MAT_NAME umat2
#define GLGE_MAT_STRUCT_NAME s_umat2
#define GLGE_MAT_VEC_TYPE uvec2
#include "Presets/GLGEMat2Preset.cpp"