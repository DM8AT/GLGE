/**
 * @file GLGECamera.h
 * @author DM8AT
 * @brief define the camera object attatchment for GLGE
 * @version 0.1
 * @date 2025-05-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef _GLGE_CAMERA_
#define _GLGE_CAMERA_

//include the settings
#include "../GLGECore/GLGESettings.h"
//include objects
#include "../GLGECore/Geometry/GLGEObject.h"

//check for C++
#if GLGE_CPP

/**
 * @brief create the camera class. Cameras are object attatchments. 
 */
class Camera : public ObjectAttatchable
{
public:

    Camera() = default;

    virtual const char* getTypeName() const noexcept {return "GLGE_CAMERA";}

protected:



};

#endif

#endif