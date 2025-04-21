/**
 * @file GLGE_OGL3_3_Texture.cpp
 * @author DM8AT
 * @brief implement the OpenGL 3.3 texture
 * @version 0.1
 * @date 2025-04-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the textures
#include "GLGE_OGL3_3_Texture.h"
//inlcude OpenGL 3.3 instances
#include "GLGE_OGL3_3_Instance.h"
//include OpenGL
#include <GL/glew.h>

void OGL3_3_Texture::createOgl(void* data, uint64_t)
{
    //extract the texture
    OGL3_3_Texture* texture = (OGL3_3_Texture*)data;

    //create a new OpenGL texture
    glGenTextures(1, &texture->m_tex);
    //activate texture unit 0
    glActiveTexture(GL_TEXTURE0);
    //bind the texture
    glBindTexture(GL_TEXTURE_2D, texture->m_tex);
    //store the selected base format
    GLenum baseFmt = GL_RGBA;
    //store the selected format
    GLenum format = GL_RGBA32F;
    //check if the texture purpose is as a render target or image
    if ((texture->m_purpose == TEXTURE_PURPOSE_IMAGE) || (texture->m_purpose == TEXTURE_PURPOSE_RENDER))
    {
        //check the texture format
        if (texture->m_isHDR)
        {
            //use the 32 bit floating point format
            if (texture->m_hasAlpha)
            {
                //also use a alpha channel, so change nothing
            }
            else
            {
                //do not use an alpha channel
                baseFmt = GL_RGB;
                format = GL_RGB32F;
            }
        }
        else
        {
            //use the 8 bit unsigned integer format
            if (texture->m_hasAlpha)
            {
                //also use an alpha channel
                format = GL_RGBA8;
            }
            else
            {
                //do not use an alpha channel
                baseFmt = GL_RGB;
                format = GL_RGB8;
            }
        }
    }
    //check for the depth format
    else if (texture->m_purpose == TEXTURE_PURPOSE_DEPTH)
    {
        //set the format to the depth format
        format = GL_DEPTH_COMPONENT32F;
        baseFmt = GL_DEPTH_COMPONENT;
    }
    //create the texture
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture->m_size.x, texture->m_size.y, 0, baseFmt, GL_UNSIGNED_BYTE, texture->m_data);

    //set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void OGL3_3_Texture::deleteOgl(void* data, uint64_t)
{
    //extract the texture
    OGL3_3_Texture* texture = (OGL3_3_Texture*)data;

    //delete the texture
    glDeleteTextures(1, &texture->m_tex);
}

void OGL3_3_Texture::mipMap(void* data, uint64_t)
{
    //extract the texture
    OGL3_3_Texture* texture = (OGL3_3_Texture*)data;

    //bind the texture to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->m_tex);
    //create the mip chain
    glGenerateMipmap(GL_TEXTURE_2D);
}

void OGL3_3_Texture::onCreate()
{
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL3_3_Instance*)m_graphicInstance)->getDataBuffer();
    //start the command buffer recording
    cmdBuff.begin();
    //queue the create function
    cmdBuff.add(0, (void*)createOgl, this, sizeof(this));
    //end the recording
    cmdBuff.end();
}

void OGL3_3_Texture::onDestroy()
{
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL3_3_Instance*)m_graphicInstance)->getDataBuffer();
    //start the command buffer recording
    cmdBuff.begin();
    //queue the delete function
    cmdBuff.add(0, (void*)deleteOgl, this, sizeof(this));
    //end the recording
    cmdBuff.end();
}

void OGL3_3_Texture::createMipMap() noexcept
{
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL3_3_Instance*)m_graphicInstance)->getDataBuffer();
    //start the command buffer recording
    cmdBuff.begin();
    //queue the mip map function
    cmdBuff.add(0, (void*)mipMap, this, sizeof(this));
    //end the recording
    cmdBuff.end();
}