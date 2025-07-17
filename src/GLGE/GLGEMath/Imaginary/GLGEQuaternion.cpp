/**
 * @file GLGEQuaternion.cpp
 * @author DM8AT
 * @brief implement the C binding for quaternions
 * @version 0.1
 * @date 2025-07-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include quaternions
#include "GLGEQuaternion.h"

//export the function declarations for C
extern "C"
{

Quaternion GLGE_C_FUNC(quaternion_Add)(const Quaternion q, const Quaternion p)
{
    //just add them
    return q + p;
}

Quaternion GLGE_C_FUNC(quaternion_Subtract)(const Quaternion q, const Quaternion p)
{
    //return the difference
    return q - p;
}

Quaternion GLGE_C_FUNC(quaternion_Multiply)(const Quaternion q, const Quaternion p)
{
    //return the product
    return q * p;
}

Quaternion GLGE_C_FUNC(quaternion_Invert)(const Quaternion q)
{
    //return the inverted quaternion
    return -q;
}

void GLGE_C_FUNC(quaternion_AddTo)(Quaternion* q, const Quaternion p)
{
    //add to the first one
    *q += p;
}

void GLGE_C_FUNC(quaternion_SubtractFrom)(Quaternion* q, const Quaternion p)
{
    //subtract from the first one
    *q -= p;
}

} //end of C section