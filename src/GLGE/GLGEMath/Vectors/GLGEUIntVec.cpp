/**
 * @file GLGEUIntVec.cpp
 * @author DM8AT
 * @brief implement the functions for the unsigned integer vectors
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the header
#include "GLGEUIntVec.h"

//define the type of the next vector to create
#define GLGE_VEC_TYPE uint32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME uvec2
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_uvec2

//inclue the vector template for 2D vectors
#include "Presets/GLGEVec2Preset.cpp"

//define the type of the next vector to create
#define GLGE_VEC_TYPE uint32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME uvec3
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_uvec3

//inclue the vector template for 3D vectors
#include "Presets/GLGEVec3Preset.cpp"

//define the type of the next vector to create
#define GLGE_VEC_TYPE uint32_t
//define the name of the next vector to create
#define GLGE_VEC_NAME uvec4
//dfine the name of the next struct to create
#define GLGE_VEC_STRUCT_NAME s_uvec4

//inclue the vector template for 4D vectors
#include "Presets/GLGEVec4Preset.cpp"
