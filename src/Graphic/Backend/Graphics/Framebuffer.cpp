/**
 * @file Framebuffer.cpp
 * @author DM8AT
 * @brief implement the behaviour for the backend-agnostic framebuffer API
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the abstract framebuffer
#include "Graphic/Backend/Graphics/Framebuffer.h"

GLGE::Graphic::Backend::Graphic::Framebuffer::Framebuffer(u8 colorAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                                                          u8 depthAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                                                          u8 stencilAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                                                          bool combinedDepthStencil, GLGE::Graphic::Backend::Graphic::Instance* instance)
 : m_hasDepthStencil(combinedDepthStencil), m_instance(instance)
{
    #if GLGE_DEBUG
    //sanity check hard limits
    if (colorAttachmentCount > MAX_COLOR_ATTACHMENTS)
    {throw GLGE::Exception("Tried to add more color attachment than hard color attachment cap", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
    if (depthAttachmentCount > MAX_DEPTH_ATTACHMENTS)
    {throw GLGE::Exception("Tried to add more depth / depth stencil attachment than hard depth / depth stencil attachment cap", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
    if (stencilAttachmentCount > MAX_STENCIL_ATTACHMENTS)
    {throw GLGE::Exception("Tried to add more stencil attachment than hard stencil attachment cap", "GLGE::Graphic::Backend::Graphic::Framebuffer");}

    //sanity check that all sizes match
    uvec2 target = colorAttachments ? colorAttachments[0]->getSize() : (depthAttachments ? depthAttachments[0]->getSize() : stencilAttachments[0]->getSize());
    u8 samples = colorAttachments ? colorAttachments[0]->getSamplesPerPixel() : (depthAttachments ? depthAttachments[0]->getSamplesPerPixel() : stencilAttachments[0]->getSamplesPerPixel());
    for (u8 i = 0; i < colorAttachmentCount; ++i) {
        if (colorAttachments[i]->getSize().x != target.x || colorAttachments[i]->getSize().y != target.y) 
        {throw GLGE::Exception("Found color attachment with differing sizes", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
        if (colorAttachments[i]->getSamplesPerPixel() != samples)
        {throw GLGE::Exception("Found color attachment with different samples per pixel", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
    }
    for (u8 i = 0; i < depthAttachmentCount; ++i) {
        if (depthAttachments[i]->getSize().x != target.x || depthAttachments[i]->getSize().y != target.y) 
        {throw GLGE::Exception("Found depth / depth stencil attachment with differing sizes", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
        if (depthAttachments[i]->getSamplesPerPixel() != samples)
        {throw GLGE::Exception("Found depth / depth stencil attachment with different samples per pixel", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
    }
    for (u8 i = 0; i < stencilAttachmentCount; ++i) {
        if (stencilAttachments[i]->getSize().x != target.x || stencilAttachments[i]->getSize().y != target.y) 
        {throw GLGE::Exception("Found stencil attachment with differing sizes", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
        if (stencilAttachments[i]->getSamplesPerPixel() != samples)
        {throw GLGE::Exception("Found stencil attachment with different samples per pixel", "GLGE::Graphic::Backend::Graphic::Framebuffer");}
    }
    #endif

    //clip to sane limits
    m_colorAttachmentCount = (colorAttachmentCount > MAX_COLOR_ATTACHMENTS) ? MAX_COLOR_ATTACHMENTS : colorAttachmentCount;
    m_depthAttachmentCount = (depthAttachmentCount > MAX_DEPTH_ATTACHMENTS) ? MAX_DEPTH_ATTACHMENTS : depthAttachmentCount;
    m_stencilAttachmentCount = (stencilAttachmentCount > MAX_STENCIL_ATTACHMENTS) ? MAX_STENCIL_ATTACHMENTS : stencilAttachmentCount;

    //store all the attachments
    for (size_t i = 0; i < m_colorAttachmentCount; ++i)
    {m_attachments[i + COLOR_IDX_OFFSET] = colorAttachments[i]; colorAttachments[i]->registerFramebuffer(this);}
    for (size_t i = 0; i < m_depthAttachmentCount; ++i)
    {m_attachments[i + DEPTH_IDX_OFFSET] = depthAttachments[i]; depthAttachments[i]->registerFramebuffer(this);}
    for (size_t i = 0; i < m_stencilAttachmentCount; ++i)
    {m_attachments[i + STENCIL_IDX_OFFSET] = stencilAttachments[i]; stencilAttachments[i]->registerFramebuffer(this);}
}

GLGE::Graphic::Backend::Graphic::Framebuffer::~Framebuffer() {
    //remove all attachments
    for (size_t i = 0; i < sizeof(m_attachments)/sizeof(*m_attachments); ++i) 
    {if (m_attachments[i].get()) {m_attachments[i]->removeFramebuffer(this);}}
}

void GLGE::Graphic::Backend::Graphic::Framebuffer::resize(const uvec2& size) {
    //store how many are queued to resize
    m_resizeCountLeft = m_colorAttachmentCount + m_depthAttachmentCount + m_stencilAttachmentCount;

    //iterate over all existing attachments and resize them
    for (u8 i = 0; i < m_colorAttachmentCount; ++i)
    {--m_resizeCountLeft; m_attachments[i + COLOR_IDX_OFFSET]->resizeAndClear(size);}
    for (u8 i = 0; i < m_depthAttachmentCount; ++i)
    {--m_resizeCountLeft; m_attachments[i + DEPTH_IDX_OFFSET]->resizeAndClear(size);}
    for (u8 i = 0; i < m_stencilAttachmentCount; ++i)
    {--m_resizeCountLeft; m_attachments[i + STENCIL_IDX_OFFSET]->resizeAndClear(size);}
}