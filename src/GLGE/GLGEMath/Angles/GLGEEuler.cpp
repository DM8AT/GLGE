/**
 * @file GLGEEuler.cpp
 * @author DM8AT
 * @brief implement the C binding for euler angles
 * @version 0.1
 * @date 2025-07-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include euler angles
#include "GLGEEuler.h"

//everything is available for C
extern "C"
{

//structure of C bindings: Just call the C++ function

Euler GLGE_C_FUNC(euler_Add)(const Euler a, const Euler b) { return a + b; }

Euler GLGE_C_FUNC(euler_AddVec3)(const Euler a, const vec3 b) { return a + b; }

Euler GLGE_C_FUNC(euler_AddDVec3)(const Euler a, const dvec3 b) { return a + b; }

Euler GLGE_C_FUNC(euler_Subtract)(const Euler a, const Euler b) { return a - b; }

Euler GLGE_C_FUNC(euler_SubtractVec3)(const Euler a, const vec3 b) { return a - b; }

Euler GLGE_C_FUNC(euler_SubtractDVec3)(const Euler a, const dvec3 b) { return a - b; }

void GLGE_C_FUNC(euler_AddTo)(Euler* a, const Euler b) { *a += b; }

void GLGE_C_FUNC(euler_AddVec3To)(Euler* a, const vec3 b) { *a += b; }

void GLGE_C_FUNC(euler_AddDVec3To)(Euler* a, const dvec3 b) { *a += b; }

void GLGE_C_FUNC(euler_SubtractFrom)(Euler* a, const Euler b) { *a -= b; }

void GLGE_C_FUNC(euler_SubtractVec3From)(Euler* a, const vec3 b) { *a -= b; }

void GLGE_C_FUNC(euler_SubtractDVec3From)(Euler* a, const dvec3 b) { *a -= b; }

Euler GLGE_C_FUNC(euler_invert)(const Euler euler) { return -euler; }

vec3 GLGE_C_FUNC(euler_asVec3)(const Euler euler) { return euler.asVec3(); }

dvec3 GLGE_C_FUNC(euler_asDVec3)(const Euler euler) { return euler.asDVec3(); }

}