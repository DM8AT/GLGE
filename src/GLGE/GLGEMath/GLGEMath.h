/**
 * @file GLGEMath.h
 * @author DM8AT
 * @brief include all sub-modules and classes for the math module of GLGE
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_MATH_
#define _GLGE_MATH_

//incldue the settings
#include "../GLGECore/GLGESettings.h"

//check if the math core is enabled
#if GLGE_INCLUDE_MATH

//include all vectors
#include "Vectors/GLGEVectors.h"
//include matricies
#include "Matricies/GLGEMatrices.h"
//include angles
#include "Angles/GLGEAngles.h"
//include imaginary stuff
#include "Imaginary/GLGEImaginary.h"

#endif //math including section

#endif