/**
 * @file GLGE_OGL4_6_Framebuffer.cpp
 * @author DM8AT
 * @brief implement the functionality of an OpenGL framebuffer
 * @version 0.1
 * @date 2025-06-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the framebuffer
#include "GLGE_OGL4_6_Framebuffer.h"
//include OpenGL textures
#include "GLGE_OGL4_6_Texture.h"
//include actual framebuffers
#include "../../../GLGEFramebuffer.h"
//include OpenGL instances
#include "GLGE_OGL4_6_Instance.h"

//include OpenGL
#include <GL/glew.h>

void OGL4_6_Framebuffer::onCreate()
{
    //store the graphic instance
    m_graphicInstance = m_fbuff->getInstance()->getGraphicInstance();
    //queue the creation
    m_graphicInstance->getBuffers()[0]->add(0, (void*)ogl_create, this, sizeof(this));
}

void OGL4_6_Framebuffer::onDestroy()
{
    //stop if no framebuffer exists
    if (!m_fbuff) {return;}

    //queue the deletion
    m_graphicInstance->getBuffers()[0]->add(0, (void*)ogl_destroy, this, sizeof(this));

    //zero out the framebuffer
    m_fbuff = 0;
    //wait till the buffer is deleted
    while (m_fbuff) {std::this_thread::sleep_for(std::chrono::microseconds(10));}
}

void OGL4_6_Framebuffer::clear(GraphicCommandBuffer* cmdBuff)
{
    //record the clear command
    cmdBuff->add(0, (void*)ogl_clear, this, sizeof(this));
}

void OGL4_6_Framebuffer::resize(const uvec2& newSize) noexcept
{
    //if the size is the current size, stop
    if ((newSize.x == m_fbuff->getSize().x) && (newSize.y == m_fbuff->getSize().y)) {return;}
    //also stop if the graphic instance is deleted
    if (!m_graphicInstance) {return;}
    //get the command buffer
    GraphicCommandBuffer& cmdBuff = ((OGL4_6_Instance*)m_graphicInstance)->getDataBuffer();

    //queue the resizing of the resizing
    cmdBuff.add(0, (void*)ogl_resize, this, *((uint64_t*)&newSize));
}

void OGL4_6_Framebuffer::ogl_create(OGL4_6_Framebuffer* fbuff, uint64_t) noexcept
{
    //create the actual framebuffer object
    glCreateFramebuffers(1, &fbuff->m_OglFbuff);

    //iterate over all color attatchments
    for (uint64_t i = 0; i < fbuff->m_fbuff->getColorAttatchmentCount(); ++i)
    {
        //add the texture to the framebuffer
        glNamedFramebufferTexture(fbuff->m_OglFbuff, GL_COLOR_ATTACHMENT0+i, 
            ((OGL4_6_Texture*)fbuff->m_fbuff->getColorAttatchment(i)->getGraphicTexture())->getOglTexture(), 0);
    }

    //check if a depth buffer exists
    if (fbuff->m_fbuff->getDepthAttatchment())
    {
        //if it does, add it
        glNamedFramebufferTexture(fbuff->m_OglFbuff, GL_DEPTH_ATTACHMENT, 
            ((OGL4_6_Texture*)fbuff->m_fbuff->getDepthAttatchment())->getOglTexture(), 0);
    }

    //check if the framebuffer was created successfully
    if (glCheckNamedFramebufferStatus(fbuff->m_OglFbuff, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        //delete the framebuffer
        glDeleteFramebuffers(1, &fbuff->m_OglFbuff);
        fbuff->m_OglFbuff = 0;

        //say that the creation failed
        fbuff->m_fbuff->getInstance()->log("Failed to create OpenGL framebuffer", MESSAGE_TYPE_ERROR);
    }
}

void OGL4_6_Framebuffer::ogl_destroy(OGL4_6_Framebuffer* fbuff, uint64_t) noexcept
{
    //delete the framebuffer
    glDeleteFramebuffers(1, &fbuff->m_OglFbuff);
    fbuff->m_fbuff = 0;
}

void OGL4_6_Framebuffer::ogl_clear(OGL4_6_Framebuffer* fbuff, uint64_t) noexcept
{
    //get the RGB color
    vec4 col = fbuff->m_fbuff->getClearColor().getInSpace(COLOR_SPACE_RGBA);
    //clear the framebuffer with the correct color
    glClearNamedFramebufferfv(fbuff->m_OglFbuff, GL_COLOR, 0, (float*)&col);
}

void OGL4_6_Framebuffer::ogl_resize(OGL4_6_Framebuffer* fbuff, uint64_t nSize) noexcept
{
    //unpack the size
    uvec2 size = *((uvec2*)&nSize);

    //create a new framebuffer and delete the old one
    glDeleteFramebuffers(1, &fbuff->m_OglFbuff);
    glCreateFramebuffers(1, &fbuff->m_OglFbuff);

    //iterate over all color attatchments
    for (uint64_t i = 0; i < fbuff->m_fbuff->getColorAttatchmentCount(); ++i)
    {
        //get the texture
        OGL4_6_Texture* tex = (OGL4_6_Texture*)fbuff->m_fbuff->getColorAttatchment(i)->getGraphicTexture();
        //change the size of the color attatchment directly
        tex->setParentSize(size);
        tex->ogl_resize(tex, sizeof(tex));
        //then add the color attatchment
        glNamedFramebufferTexture(fbuff->m_OglFbuff, GL_COLOR_ATTACHMENT0+i, tex->m_tex, 0);
    }
    //extract the depth attachment
    Texture* depth = (Texture*)fbuff->m_fbuff->getDepthAttatchment();
    //resize the depth buffer if it exists
    if (depth)
    {
        //extract the depth texture
        OGL4_6_Texture* depthTex = (OGL4_6_Texture*)depth->getGraphicTexture();
        //first, directly resize the texture
        depthTex->setParentSize(size);
        depthTex->ogl_resize(depth->getGraphicTexture(), sizeof(void*));
        //then, add it back as the depth attatchment
        glNamedFramebufferTexture(fbuff->m_OglFbuff, GL_DEPTH_ATTACHMENT, depthTex->m_tex, 0);
    }

    //check if the framebuffer was created successfully
    if (glCheckNamedFramebufferStatus(fbuff->m_OglFbuff, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        //delete the framebuffer
        glDeleteFramebuffers(1, &fbuff->m_OglFbuff);
        fbuff->m_OglFbuff = 0;

        //say that the creation failed
        fbuff->m_fbuff->getInstance()->log("Failed to resize OpenGL framebuffer", MESSAGE_TYPE_ERROR);
    }
}