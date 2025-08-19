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
//include textures
#include "../GLGETexture.h"

/**
 * @brief store a lookup table for the texture purposes
 */
const char* TEXTURE_PURPOSE_LUT[] = {
    "TEXTURE_PURPOSE_IMAGE",
    "TEXTURE_PURPOSE_RENDER",
    "TEXTURE_PURPOSE_DEPTH"
};

const char* GLGE_C_FUNC(texturePurposeToString(TexturePurpose purpose))
{
    //check if the purpose is in bounds
    if (((uint32_t)purpose) > (sizeof(TEXTURE_PURPOSE_LUT) / sizeof(*TEXTURE_PURPOSE_LUT)))
    {
        //return empty
        return 0;
    }

    //else, return the correct string
    return TEXTURE_PURPOSE_LUT[(uint32_t)purpose];
}

void GraphicTexture::create(Texture* texture, void* data, bool isHDR, bool alpha, GraphicInstance* instance)
{
    //check if the texture is set up
    if (m_graphicInstance)
    {
        //log an error
        m_graphicInstance->getInstance()->log("Can not re-create texture that was never destroyed", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the parent texture
    m_texture = texture;
    //store the inputs
    m_isHDR = isHDR;
    m_hasAlpha = alpha;
    m_graphicInstance = instance;
    //add the element to the instance
    m_graphicInstance->addElement(this);
    //calcualte the data size
    uint64_t datSize = ((m_isHDR) ? sizeof(float) : sizeof(uint8_t)) * ((m_hasAlpha) ? 4 : 3) * m_texture->getSize().x * m_texture->getSize().y;
    //create the data
    m_data = malloc(datSize);
    //copy the data
    memcpy(m_data, data, datSize);

    //call the create hook
    onCreate();
}

void GraphicTexture::create(Texture* texture, bool isHDR, bool alpha, GraphicInstance* instance)
{
    //check if the texture is set up
    if (m_graphicInstance)
    {
        //log an error
        m_graphicInstance->getInstance()->log("Can not re-create texture that was never destroyed", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the parent texture
    m_texture = texture;
    //store the inputs
    m_isHDR = isHDR;
    m_hasAlpha = alpha;
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

    //remove from the texture
    m_texture->m_texture = 0;
    //remove from the instance
    m_graphicInstance->removeElement(this);
    m_graphicInstance = 0;
    //free the data
    free(m_data);
    m_data = 0;
    //reset the variables
    m_isHDR = false;
}

void GraphicTexture::setParentSize(const uvec2& size) noexcept
{
    //store the new size to the parent
    m_texture->m_size = size;
}