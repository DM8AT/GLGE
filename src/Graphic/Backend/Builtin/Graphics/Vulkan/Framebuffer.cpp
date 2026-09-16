/**
 * @file Framebuffer.cpp
 * @author DM8AT
 * @brief implement the vulkan framebuffer backend
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the framebuffer Vulkan overload
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Framebuffer.h"
//get the vulkan instance
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add Vulkan images
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Image.h"
//add frontend framebuffers
#include "Graphic/Framebuffer.h"

//add vulkan
#include "vulkan/vulkan.h"

GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::Framebuffer(u8 colorAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                                                                  u8 depthAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                                                                  u8 stencilAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                                                                  bool combinedDepthStencil, GLGE::Graphic::Backend::Graphic::Instance* instance)
 : GLGE::Graphic::Backend::Graphic::Framebuffer(colorAttachmentCount, colorAttachments, depthAttachmentCount, depthAttachments, stencilAttachmentCount, stencilAttachments, combinedDepthStencil, instance)
{invalidate();}

GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::~Framebuffer() 
{}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::onClear(vec4 color, float depth, u32 stencil) {
    //TODO
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::onImageUpdated(GLGE::Graphic::Backend::Graphic::Image* image) {
    //if framebuffers are left to resize, ignore it
    if (m_resizeCountLeft > 0) {return;}

    invalidate();
}