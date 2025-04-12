/**
 * @file GLGE.h
 * @author DM8AT
 * @brief GLGE is a 3D and 2D graphics library
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_
#define _GLGE_

//include the settings
#include "GLGESettings.h"

//check if a namespace should be used and if this is C++
#if GLGE_CREATE_NAMESPACE && GLGE_CPP
//open the namespace
namespace GLGE_NAMESPACE {
#endif

//check if the mathematic library should be included
#if GLGE_INCLUDE_MATH

//include the vectors
#include "../GLGEMath/Vectors/GLGEVectors.h"

#endif //math inclusion section

//check if a namespace should be used and if this is C++
#if GLGE_CREATE_NAMESPACE && GLGE_CPP
//close the namespace
}
#endif

#endif