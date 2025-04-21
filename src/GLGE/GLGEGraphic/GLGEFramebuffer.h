/**
 * @file GLGEFramebuffer.h
 * @author DM8AT
 * @brief define framebuffers for GLGE
 * @version 0.1
 * @date 2025-04-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_FRAMEBUFFER_
#define _GLGE_FRAMEBUFFER_

//include the settings
#include "../GLGECore/GLGESettings.h"
//include textures
#include "GLGETexture.h"

//check if this is C++
#if GLGE_CPP

class Framebuffer
{
public:

    Framebuffer() = default;

protected:

    /**
     * @brief store all color attatchments
     */
    std::vector<Texture*> m_colorAttatchments;
    /**
     * @brief store the depth attatchments
     */
    Texture* m_depthAttatchment = 0;

};

#endif //C++ section

#endif