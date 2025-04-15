/**
 * @file GLGEGraphicAPI_Framebuffer.cpp
 * @author DM8AT
 * @brief implement the default graphic framebuffer
 * @version 0.1
 * @date 2025-04-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the framebuffer
#include "GLGEGraphicAPI_Framebuffer.h"


void GraphicFramebuffer::create(GraphicTexture* textures, uint64_t textureCount, GraphicTexture* depthBuffer, GraphicInstance* instance)
{
    //check if the framebuffer exists
    if (m_graphicInstance)
    {
        //print an error
        m_graphicInstance->getInstance()->log("Can not create framebuffer because it is allready created", MESSAGE_TYPE_ERROR);
        return;
    }

    //allocate the textures
    m_textures.resize(textureCount);
    //store the textures
    memcpy(m_textures.data(), textures, sizeof(*textures)*textureCount);
    //store the depth buffer
    m_depthBuffer = depthBuffer;

    //store the instance
    m_graphicInstance = instance;
    //add this to the instance
    m_graphicInstance->addElement(this);

    //call the attatch hook
    onAttatch();
}

void GraphicFramebuffer::destroy()
{
    //check if the framebuffer is set up
    if (!m_graphicInstance) {return;}

    //call the destroy hook
    onDestroy();

    //remove from the instance
    m_graphicInstance->removeElement(this);
    m_graphicInstance = 0;
    //free the textures
    m_textures.clear();
    //free the depth buffer
    m_depthBuffer = 0;
}