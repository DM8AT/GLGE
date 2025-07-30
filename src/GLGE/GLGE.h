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
#include "GLGECore/GLGESettings.h"

//check if a namespace should be used and if this is C++
#if GLGE_CREATE_NAMESPACE && GLGE_CPP
//open the namespace
namespace GLGE_NAMESPACE {
#endif

//include the library core
#include "GLGECore/GLGECore.h"

//include the graphic module
#include "GLGEGraphic/GLGEGraphic.h"

//include the math module
#include "GLGEMath/GLGEMath.h"

//check if a namespace should be used and if this is C++
#if GLGE_CREATE_NAMESPACE && GLGE_CPP
//close the namespace
}
#endif

#endif