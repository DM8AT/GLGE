/**
 * @file Framebuffer.cpp
 * @author DM8AT
 * @brief implement the OpenGL overload for a framebuffer
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the framebuffer OpenGL overload
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.h"
//add OpenGL images
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Image.h"
//add the mappings
#include "__Mapper.h"

GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::Framebuffer(u8 colorAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                                                                  u8 depthAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                                                                  u8 stencilAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                                                                  bool combinedDepthStencil, GLGE::Graphic::Backend::Graphic::Instance* instance)
 : GLGE::Graphic::Backend::Graphic::Framebuffer(colorAttachmentCount, colorAttachments, depthAttachmentCount, depthAttachments, stencilAttachmentCount, stencilAttachments, combinedDepthStencil, instance)
{
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::Framebuffer")

    //create the framebuffer
    glCreateFramebuffers(1, &m_handle);
    //in debug name the texture
    #if GLGE_DEBUG
    std::string name = std::string("Framebuffer_") + std::to_string(m_name);
    glObjectLabel(GL_FRAMEBUFFER, m_handle, name.length(), name.c_str());
    #endif

    //add all the color attachments
    for (u8 i = 0; i < m_colorAttachmentCount; ++i)
    {glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0 + i, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(getColorAttachment(i))->getHandle(), 0);}
    //check if a depth stencil attachment is used
    if (m_hasDepthStencil) {
        //use a depth stencil attachment
        glNamedFramebufferTexture(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(getDepthAttachment(0))->getHandle(), 0);
    } else {
        //separate the attachments
        if (m_depthAttachmentCount)
        {glNamedFramebufferTexture(m_handle, GL_DEPTH_ATTACHMENT, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(getDepthAttachment(0))->getHandle(), 0);}
        if (m_stencilAttachmentCount)
        {glNamedFramebufferTexture(m_handle, GL_STENCIL_ATTACHMENT, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(getStencilAttachment(0))->getHandle(), 0);}
    }

    //sanity check
    GLenum status = glCheckNamedFramebufferStatus(m_handle, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) 
    {throw GLGE::Exception("Tried to create a framebuffer with invalid configuration", "GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::Framebuffer");}
}

GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::~Framebuffer() {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::~Framebuffer")

    //if the framebuffer exists, delete it
    if (m_handle)
    {glDeleteFramebuffers(1, &m_handle);}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::onClear(vec4 color, float depth, u32 stencil) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::onClear")

    //iterate over all color attachments
    for (u8 i = 0; i < m_colorAttachmentCount; ++i)
    {glClearNamedFramebufferfv(m_handle, GL_COLOR, i, &color.x);}
    //check if both depth and stencil exist or a depth stencil attachment is used
    if ((m_stencilAttachmentCount && m_depthAttachmentCount) || m_hasDepthStencil) {
        //just clear both at once
        glClearNamedFramebufferfi(m_handle, GL_DEPTH_STENCIL, 0, depth, stencil);
    } else {
        //else, just clear the one that exists
        if (m_stencilAttachmentCount) {
            glClearNamedFramebufferfv(m_handle, GL_DEPTH, 0, &depth);
        } else if (m_depthAttachmentCount) {
            glClearNamedFramebufferiv(m_handle, GL_STENCIL, 0, (i32*)&stencil);
        }
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer::onImageUpdated(GLGE::Graphic::Backend::Graphic::Image* image) {
    //get the index of the image
    size_t idx = SIZE_MAX;
    for (size_t i = 0; i < sizeof(m_attachments)/sizeof(*m_attachments); ++i) {
        if (m_attachments[i].get() == image) 
        {idx = i; break;}
    }

    //check for found
    if (idx == SIZE_MAX) {return;}

    //update the image
    if (idx >= STENCIL_IDX_OFFSET) {
        //update as stencil attachment or depth stencil attachment
        if (m_hasDepthStencil) 
        {glNamedFramebufferTexture(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(image)->getHandle(), 0);} 
        else 
        {glNamedFramebufferTexture(m_handle, GL_STENCIL_ATTACHMENT, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(image)->getHandle(), 0);}
    } else if (idx >= DEPTH_IDX_OFFSET) {
        //update as depth attachment or depth stencil attachment
        if (m_hasDepthStencil) 
        {glNamedFramebufferTexture(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(image)->getHandle(), 0);} 
        else 
        {glNamedFramebufferTexture(m_handle, GL_DEPTH_ATTACHMENT, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(image)->getHandle(), 0);}
    } else {
        glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0+idx, reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(image)->getHandle(), 0);
    }
}