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
{
    //create the render pass
    createRenderPass();
    //create the framebuffer
    createFramebuffers();
}

GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::~Framebuffer() {
    //destroy the framebuffer
    destroyFramebuffers();
    //destroy the render pass
    destroyRenderPass();
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::onClear(vec4 color, float depth, u32 stencil) {
    //TODO
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::onImageUpdated(GLGE::Graphic::Backend::Graphic::Image* image) {
    //destroy the framebuffer
    destroyFramebuffers();
    //destroy the render pass
    destroyRenderPass();

    //create the render pass
    createRenderPass();
    //create the framebuffer
    createFramebuffers();
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::createRenderPass() {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //collect all attachments
    std::vector<VkAttachmentDescription> attDescr;
    attDescr.reserve(m_colorAttachmentCount + m_depthAttachmentCount);
    //store the references for all color images
    std::vector<VkAttachmentReference> colRef;
    colRef.reserve(m_colorAttachmentCount);

    //collect color attachments
    for (u8 i = 0; i < m_colorAttachmentCount; ++i) {
        //get the vulkan image backend
        auto* img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(m_attachments[i].get());

        //create the attachment description
        VkAttachmentDescription desc{};
        desc.format = static_cast<VkFormat>(img->getVkFormat());
        desc.samples = static_cast<VkSampleCountFlagBits>(img->getSamplesPerPixel());
        desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        desc.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
        desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
        attDescr.push_back(desc);

        //create the attachment reference
        VkAttachmentReference ref {};
        ref.attachment = i;
        ref.layout = VK_IMAGE_LAYOUT_GENERAL;
        colRef.push_back(ref);
    }

    //depth attachment
    VkAttachmentReference depthRef {};
    bool hasDepth = m_depthAttachmentCount > 0;
    if (hasDepth) {
        //get the vulkan image backend
        auto* img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(m_attachments[DEPTH_IDX_OFFSET].get());

        //create the attachment description
        VkAttachmentDescription desc{};
        desc.format = static_cast<VkFormat>(img->getVkFormat());
        desc.samples = static_cast<VkSampleCountFlagBits>(img->getSamplesPerPixel());
        desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        desc.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
        desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        //create the depth reference
        depthRef.attachment = static_cast<u32>(attDescr.size());
        depthRef.layout = VK_IMAGE_LAYOUT_GENERAL;

        //store the attachment description
        attDescr.push_back(desc);
    }

    //create the subpass
    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = m_colorAttachmentCount;
    subpass.pColorAttachments = colRef.data();
    subpass.pDepthStencilAttachment = hasDepth ? &depthRef : nullptr;

    //create the render pass
    VkRenderPassCreateInfo renPassCreate {};
    renPassCreate.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renPassCreate.attachmentCount = static_cast<u32>(attDescr.size());
    renPassCreate.pAttachments = attDescr.data();
    renPassCreate.subpassCount = 1;
    renPassCreate.pSubpasses = &subpass;
    VkRenderPass pass;
    if (vkCreateRenderPass(reinterpret_cast<VkDevice>(inst->getDevice()), &renPassCreate, nullptr, &pass) != VK_SUCCESS)
    {throw Exception("Failed to create the render pass", "GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::createRenderPass");}

    //store the render pass
    m_renderPass = reinterpret_cast<void*>(pass);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::destroyRenderPass() {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //if a render pass exists, delete it
    if (m_renderPass) 
    {vkDestroyRenderPass(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkRenderPass>(m_renderPass), nullptr);}
    //set render pass to null
    m_renderPass = nullptr;
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::createFramebuffers() {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //store the current size
    uvec2 size = m_attachments[0]->getSize();

    //collect the image views
    std::vector<VkImageView> views;
    views.reserve(m_colorAttachmentCount + m_depthAttachmentCount);
    //get color images
    for (u8 i = 0; i < m_colorAttachmentCount; ++i) {
        //get the vulkan image backend
        auto* img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(m_attachments[i].get());
        //store the view
        views.push_back(reinterpret_cast<VkImageView>(img->getView()));

        //sanity check the size in debug
        #if GLGE_DEBUG
        if (size.x != img->getSize().x || size.y != img->getSize().y)
        {throw Exception("Size mismatch between images for a framebuffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::createFramebuffers");}
        #endif
    }
    //if a depth buffer exists, store it
    if (m_depthAttachmentCount > 0) {
        //get the vulkan image backend
        auto* img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(m_attachments[DEPTH_IDX_OFFSET].get());
        //store the depth image view
        views.push_back(reinterpret_cast<VkImageView>(img->getView()));

        //sanity check the size in debug
        #if GLGE_DEBUG
        if (size.x != img->getSize().x || size.y != img->getSize().y)
        {throw Exception("Size mismatch between images for a framebuffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::createFramebuffers");}
        #endif
    }

    //create the framebuffer
    VkFramebufferCreateInfo fbCreate {};
    fbCreate.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbCreate.renderPass = reinterpret_cast<VkRenderPass>(m_renderPass);
    fbCreate.attachmentCount = static_cast<u32>(views.size());
    fbCreate.pAttachments = views.data();
    fbCreate.width = size.x;
    fbCreate.height = size.y;
    fbCreate.layers = 1;
    VkFramebuffer fbuff;
    if (vkCreateFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), &fbCreate, nullptr, &fbuff) != VK_SUCCESS)
    {throw Exception("Failed to create a framebuffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::createFramebuffers");}

    //store the framebuffer
    m_vkFramebuffer = reinterpret_cast<void*>(fbuff);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer::destroyFramebuffers() {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //if a framebuffer exists, delete it
    if (m_vkFramebuffer)
    {vkDestroyFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkFramebuffer>(m_vkFramebuffer), nullptr);}
    //set the framebuffer to null
    m_vkFramebuffer = nullptr;
}