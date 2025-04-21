/**
 * @file GLGEGraphicAPI_Texture.cpp
 * @author DM8AT
 * @brief implement the default states for the graphic texture
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the graphic texture
#include "GLGEGraphicAPI_Texture.h"

void GraphicTexture::create(void* data, bool isHDR, bool alpha, const uvec2& size, GraphicInstance* instance)
{
    //check if the texture is set up
    if (m_graphicInstance)
    {
        //log an error
        m_graphicInstance->getInstance()->log("Can not re-create texture that was never destroyed", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the inputs
    m_isHDR = isHDR;
    m_hasAlpha = alpha;
    m_size = size;
    m_graphicInstance = instance;
    //add the element to the instance
    m_graphicInstance->addElement(this);
    //calcualte the data size
    uint64_t datSize = ((m_isHDR) ? sizeof(float) : sizeof(uint8_t)) * ((m_hasAlpha) ? 4 : 3) * m_size.x * m_size.y;
    //create the data
    m_data = malloc(datSize);
    //copy the data
    memcpy(m_data, data, datSize);

    //call the create hook
    onCreate();
}

void GraphicTexture::create(TexturePurpose purpose, const uvec2& size, bool isHDR, bool alpha, GraphicInstance* instance)
{
    //check if the texture is set up
    if (m_graphicInstance)
    {
        //log an error
        m_graphicInstance->getInstance()->log("Can not re-create texture that was never destroyed", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the inputs
    m_purpose = purpose;
    m_isHDR = isHDR;
    m_hasAlpha = alpha;
    m_size = size;
    m_graphicInstance = instance;
    //add the element to the instance
    m_graphicInstance->addElement(this);

    //call the create hook
    onCreate();
}

void GraphicTexture::destroy()
{
    //check if the texture is set up
    if (!m_graphicInstance) {return;}

    //call the destroy hook
    onDestroy();

    //remove from the instance
    m_graphicInstance->removeElement(this);
    m_graphicInstance = 0;
    //free the data
    free(m_data);
    m_data = 0;
    //reset the variables
    m_isHDR = false;
    m_size = 0;
}