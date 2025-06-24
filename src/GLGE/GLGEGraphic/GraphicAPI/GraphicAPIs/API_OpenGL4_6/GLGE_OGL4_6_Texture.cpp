/**
 * @file GLGE_OGL4_6_Texture.cpp
 * @author DM8AT
 * @brief implement the OpenGL 4.6 texture
 * @version 0.1
 * @date 2025-04-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the textures
#include "GLGE_OGL4_6_Texture.h"
//inlcude OpenGL 4.6 instances
#include "GLGE_OGL4_6_Instance.h"
//include textures
#include "../../../GLGETexture.h"
//include buffer
#include "../../../GLGEBuffer.h"
//include OpenGL
#include <GL/glew.h>

/**
 * @brief Utility function to compute the OpenGL format
 * 
 * @param format stores the format of the texture
 * @param baseFmt stores the base format (like count of channels) for the texture
 * @param purpose the GLSL purpose of the texture
 * @param isHDR true : Floats are used for the channels | false : Uint8's are used for the channels
 * @param alpha true : 4 channels per pixel | false : 3 channels per pixel
 */
void getOglFormats(GLenum& format, GLenum& baseFmt, TexturePurpose purpose, bool isHDR, bool alpha)
{
    //store the selected base format
    baseFmt = GL_RGBA;
    //store the selected format
    format = GL_RGBA32F;
    //check if the texture purpose is as a render target or image
    if ((purpose == TEXTURE_PURPOSE_IMAGE) || (purpose == TEXTURE_PURPOSE_RENDER))
    {
        //check the texture format
        if (isHDR)
        {
            //use the 32 bit floating point format
            if (alpha)
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
            if (alpha)
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
    else if (purpose == TEXTURE_PURPOSE_DEPTH)
    {
        //set the format to the depth format
        format = GL_DEPTH_COMPONENT32F;
        baseFmt = GL_DEPTH_COMPONENT;
    }
}

void OGL4_6_Texture::createOgl(void* data, uint64_t)
{
    //extract the texture
    OGL4_6_Texture* texture = (OGL4_6_Texture*)data;

    //if the texture's purpose is CPU only, stop. No GPU texture is needed. 
    if (texture->m_texture->getPurpose() == TEXTURE_PURPOSE_CPU_ONLY) {return;}

    //create a new OpenGL texture
    glGenTextures(1, &texture->m_tex);
    //activate texture unit 0
    glActiveTexture(GL_TEXTURE0);
    //bind the texture
    glBindTexture(GL_TEXTURE_2D, texture->m_tex);

    //compute the format
    //store the selected base format
    GLenum baseFmt = GL_RGBA;
    //store the selected format
    GLenum format = GL_RGBA32F;
    //extract the format
    getOglFormats(format, baseFmt, texture->getTexture()->getPurpose(), texture->m_isHDR, texture->m_hasAlpha);

    //create the texture
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture->m_texture->getSize().x, texture->m_texture->getSize().y, 0, baseFmt, GL_UNSIGNED_BYTE, texture->m_data);

    //set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //create the handle from a texture
    texture->m_handle = glGetTextureHandleARB(texture->m_tex);
    //store the texture handle in the data
    ((OGL4_6_Instance*)texture->m_graphicInstance)->getTextureBuffer()->getMemoryArena()->update(texture->m_ptr, &texture->m_handle);

    //only create an image handle if the format has an alpha channel (OpenGL dosn't like images without alpha)
    if (texture->m_hasAlpha)
    {
        //get the new image handle
        texture->m_image = glGetImageHandleARB(texture->m_tex, 0, GL_FALSE, 0, format);
        //store the image handle in the data
        ((OGL4_6_Instance*)texture->m_graphicInstance)->getImageBuffer()->getMemoryArena()->update(texture->m_imgPtr, &texture->m_image);
    }
    else
    {
        //else, make sure to set the handle to 0
        texture->m_image = 0;
    }
}

void OGL4_6_Texture::deleteOgl(void* data, uint64_t)
{
    //extract the texture
    OGL4_6_Texture* texture = (OGL4_6_Texture*)data;

    //make sure to free the texture and image pointer
    ((OGL4_6_Instance*)texture->m_graphicInstance)->getTextureBuffer()->getMemoryArena()->release(texture->m_ptr);
    ((OGL4_6_Instance*)texture->m_graphicInstance)->getImageBuffer()->getMemoryArena()->release(texture->m_imgPtr);

    //delete the texture
    glDeleteTextures(1, &texture->m_tex);
}

void OGL4_6_Texture::ogl_resize(void* data, uint64_t) noexcept
{
    //extract the texture and size
    OGL4_6_Texture* tex = (OGL4_6_Texture*)data;

    //compute the format
    //store the selected base format
    GLenum baseFmt = GL_RGBA;
    //store the selected format
    GLenum format = GL_RGBA32F;
    //extract the format
    getOglFormats(format, baseFmt, tex->getTexture()->getPurpose(), tex->m_isHDR, tex->m_hasAlpha);

    //create some temporary memory to zero out the texture
    uint64_t datSize = tex->m_texture->getSize().x * tex->m_texture->getSize().y * sizeof(uint8_t) * (tex->m_hasAlpha ? 4 : 3);
    uint8_t* dat = (uint8_t*)malloc(datSize);
    bzero(dat, datSize);

    //make the handle non-resident if it is
    if (glIsTextureHandleResidentARB(tex->m_handle)) {glMakeTextureHandleNonResidentARB(tex->m_handle);}
    //texture must be deleted and created new again for the handle to work correctly
    //first, delete the texture
    glDeleteTextures(1, &tex->m_tex);

    //create a new OpenGL texture
    glGenTextures(1, &tex->m_tex);
    //bind the texture to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->m_tex);

    //set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //update the size
    glTexImage2D(GL_TEXTURE_2D, 0, format, tex->m_texture->getSize().x, tex->m_texture->getSize().y, 0, baseFmt, GL_UNSIGNED_BYTE, dat);

    //get the new texture handle
    tex->m_handle = glGetTextureHandleARB(tex->m_tex);
    //store the texture handle in the data
    ((OGL4_6_Instance*)tex->m_graphicInstance)->getTextureBuffer()->getMemoryArena()->update(tex->m_ptr, &tex->m_handle);

    //get the new image handle
    tex->m_image = glGetImageHandleARB(tex->m_tex, 0, GL_FALSE, 0, format);
    //store the image handle in the data
    ((OGL4_6_Instance*)tex->m_graphicInstance)->getImageBuffer()->getMemoryArena()->update(tex->m_imgPtr, &tex->m_image);

    //make sure to clean up the data
    free(dat);
}

void OGL4_6_Texture::mipMap(void* data, uint64_t)
{
    //extract the texture
    OGL4_6_Texture* texture = (OGL4_6_Texture*)data;

    //bind the texture to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->m_tex);
    //create the mip chain
    glGenerateMipmap(GL_TEXTURE_2D);
}

void OGL4_6_Texture::onCreate()
{
    OGL4_6_Instance* inst = (OGL4_6_Instance*)m_graphicInstance;
    //allocate a reagion of the texture and image buffer
    m_ptr = inst->getTextureBuffer()->getMemoryArena()->allocate(sizeof(m_handle));
    m_imgPtr = inst->getImageBuffer()->getMemoryArena()->allocate(sizeof(m_image));
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL4_6_Instance*)m_graphicInstance)->getDataBuffer();
    //start the command buffer recording
    cmdBuff.begin();
    //queue the create function
    cmdBuff.add(0, (void*)createOgl, this, sizeof(this));
    //end the recording
    cmdBuff.end();
}

void OGL4_6_Texture::onDestroy()
{
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL4_6_Instance*)m_graphicInstance)->getDataBuffer();
    //start the command buffer recording
    cmdBuff.begin();
    //queue the delete function
    cmdBuff.add(0, (void*)deleteOgl, this, sizeof(this));
    //end the recording
    cmdBuff.end();
}

void OGL4_6_Texture::createMipMap() noexcept
{
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL4_6_Instance*)m_graphicInstance)->getDataBuffer();
    //queue the mip map function
    cmdBuff.add(0, (void*)mipMap, this, sizeof(this));
}

void OGL4_6_Texture::resize(const uvec2& newSize) noexcept
{
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL4_6_Instance*)m_graphicInstance)->getDataBuffer();
    //store the new size
    setParentSize(newSize);
    //queue the size change
    cmdBuff.add(0, (void*)ogl_resize, this, *((uint64_t*)&newSize));
}

void OGL4_6_Texture::activate() noexcept
{
    //make the handles resident
    glMakeTextureHandleResidentARB(m_handle);
    //only make the image handle resident if it exists
    if (m_image != 0) {glMakeImageHandleResidentARB(m_image, GL_READ_WRITE);}
}

void OGL4_6_Texture::deactivate() noexcept
{
    //simply make the handles non-resident
    glMakeTextureHandleNonResidentARB(m_handle);
    //only make the image non-handle resident if it exists
    if (m_image != 0) {glMakeImageHandleNonResidentARB(m_image);}
}