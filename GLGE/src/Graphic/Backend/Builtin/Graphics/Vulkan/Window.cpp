/**
 * @file Window.cpp
 * @author DM8AT
 * @brief implement the vulkan window backend
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the Vulkan window
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"

//include Vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add video windows
#include "Graphic/Backend/Video/Window.h"

//get the vulkan contract
#include "Graphic/Backend/Video/APIContracts/Vulkan.h"

//include profiling
#include "Core/Profiler.h"

//add vulkan
#include "vulkan/vulkan.h"

//use the namespace
using namespace GLGE::Graphic::Backend::Graphic::Vulkan;

Window::Window(GLGE::Graphic::Window* window) 
 : GLGE::Graphic::Backend::Graphic::Window(window)
{
    //setup the window
    onWindowSetup();
}

Window::~Window() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getWindow()->getGraphicInstance()->getGraphicBackendInstance().get());

    //if image views exist, destroy them
    for (auto* views : m_imgViews) 
    {vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(views), nullptr);}
    m_imgViews.clear();

    //destroy the swapchain
    vkDestroySwapchainKHR(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSwapchainKHR>(m_swapchain), nullptr);

    //destroy the window surface
    getWindow()->getGraphicInstance()->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::Vulkan>()->destroyWindowSurface(
        inst->getInstance(), m_surface, getWindow()->getVideoWindow()
    );
    //make sure the surface is gone
    vkDestroySurfaceKHR(reinterpret_cast<VkInstance>(inst->getInstance()), reinterpret_cast<VkSurfaceKHR>(m_surface), nullptr);
}

void Window::onWindowSetup() {
    //just set the resolution
    onResolutionChange(getWindow()->getSize(), getWindow()->getUsableSize(), getWindow()->getResolution());
}

void Window::onResolutionChange(const uvec2& size, const uvec2& newUsableSize, const uvec2& res) {
    GLGE_PROFILER_SCOPE();

    //early out if nothing changed
    if (res.x == m_resolution.x && res.y == m_resolution.y)
    {return;}

    //resized
    m_resized = true;

    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getWindow()->getGraphicInstance()->getGraphicBackendInstance().get());

    //the queue must be idle before the window can delete the images
    vkQueueWaitIdle(reinterpret_cast<VkQueue>(inst->getGraphicsQueue().queues[0].first));

    //if image views exist, destroy them
    for (auto* views : m_imgViews) 
    {vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(views), nullptr);}
    m_imgViews.clear();

    //if a swapchain exists, destroy it
    if (m_swapchain) 
    {vkDestroySwapchainKHR(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSwapchainKHR>(m_swapchain), nullptr);}

    //destroy the window surface
    getWindow()->getGraphicInstance()->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::Vulkan>()->destroyWindowSurface(
        reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getWindow()->getGraphicInstance()->getGraphicBackendInstance().get())->getInstance(), 
        m_surface,
        getWindow()->getVideoWindow()
    );
    
    //get the window surface
    getWindow()->getGraphicInstance()->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::Vulkan>()->createWindowSurface(
        inst->getInstance(), 
        &m_surface,
        getWindow()->getVideoWindow()
    );

    //get the properties of the surface
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(reinterpret_cast<VkPhysicalDevice>(inst->getPhysicalDevice()), reinterpret_cast<VkSurfaceKHR>(m_surface), &caps);

    //store the choosen image format
    VkFormat choosenFormat = VK_FORMAT_B8G8R8A8_UNORM;
    m_format = static_cast<i32>(choosenFormat);
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    //create the swapchain
    VkSwapchainCreateInfoKHR swapCreate {};
    swapCreate.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapCreate.surface = reinterpret_cast<VkSurfaceKHR>(m_surface);
    swapCreate.minImageCount = caps.minImageCount + 1;
    swapCreate.imageFormat = choosenFormat;
    swapCreate.imageColorSpace = colorSpace;
    swapCreate.imageExtent.width = res.x;
    swapCreate.imageExtent.height = res.y;
    swapCreate.imageArrayLayers = 1;
    swapCreate.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    swapCreate.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapCreate.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapCreate.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (vkCreateSwapchainKHR(reinterpret_cast<VkDevice>(inst->getDevice()), &swapCreate, nullptr, reinterpret_cast<VkSwapchainKHR*>(&m_swapchain)) != VK_SUCCESS)
    {throw Exception("Failed to create the swapchain", "GLGE::Graphic::Backend::Graphic::Vulkan::Window");}

    //get all the swapchain images
    u32 imgCount = 0;
    vkGetSwapchainImagesKHR(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSwapchainKHR>(m_swapchain), &imgCount, nullptr);
    m_imgs.resize(imgCount);
    vkGetSwapchainImagesKHR(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSwapchainKHR>(m_swapchain), &imgCount, reinterpret_cast<VkImage*>(m_imgs.data()));

    //create image views for all images
    m_imgViews.reserve(m_imgs.size());
    for (size_t i = 0; i < m_imgs.size(); ++i) {
        //create the image view
        VkImageViewCreateInfo imgViewCreate {};
        imgViewCreate.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgViewCreate.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgViewCreate.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgViewCreate.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgViewCreate.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgViewCreate.format = choosenFormat;
        imgViewCreate.image = reinterpret_cast<VkImage>(m_imgs[i]);
        imgViewCreate.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgViewCreate.subresourceRange.baseArrayLayer = 0;
        imgViewCreate.subresourceRange.layerCount = 1;
        imgViewCreate.subresourceRange.baseMipLevel = 0;
        imgViewCreate.subresourceRange.levelCount = 1;
        imgViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D;
        VkImageView view;
        if (vkCreateImageView(reinterpret_cast<VkDevice>(inst->getDevice()), &imgViewCreate, nullptr, &view) != VK_SUCCESS)
        {throw Exception("Failed to create an image view", "GLGE::Graphic::Backend::Graphic::Vulkan::Window");}
        m_imgViews.push_back(reinterpret_cast<void*>(view));
    }
    
    //use a temporary command pool and command buffer to set the layout of the images correctly
    VkCommandPool tempPool;
    VkCommandPoolCreateInfo tmpPoolCreate {};
    tmpPoolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    tmpPoolCreate.queueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    if (vkCreateCommandPool(reinterpret_cast<VkDevice>(inst->getDevice()), &tmpPoolCreate, nullptr, &tempPool) != VK_SUCCESS)
    {throw Exception("Failed to create a temporary command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::Window");}
    //get a temporary command buffer
    VkCommandBufferAllocateInfo cmdBuffAlloc {};
    cmdBuffAlloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBuffAlloc.commandBufferCount = 1;
    cmdBuffAlloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBuffAlloc.commandPool = tempPool;
    VkCommandBuffer cmdBuff;
    vkAllocateCommandBuffers(reinterpret_cast<VkDevice>(inst->getDevice()), &cmdBuffAlloc, &cmdBuff);

    //create a dummy render pass to just clear the windows
    VkAttachmentDescription attachDescr {};
    attachDescr.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachDescr.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachDescr.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachDescr.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachDescr.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachDescr.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachDescr.format = choosenFormat;
    attachDescr.samples = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentReference attachRef {};
    attachRef.attachment = 0;
    attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass {};
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachRef;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    VkRenderPassCreateInfo renPassCreate {};
    renPassCreate.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renPassCreate.attachmentCount = 1;
    renPassCreate.pAttachments = &attachDescr;
    renPassCreate.subpassCount = 1;
    renPassCreate.pSubpasses = &subpass;
    VkRenderPass initPass;
    if (vkCreateRenderPass(reinterpret_cast<VkDevice>(inst->getDevice()), &renPassCreate, nullptr, &initPass) != VK_SUCCESS)
    {throw Exception("Failed to create the dummy render pass", "GLGE::Graphic::Backend::Graphic::Vulkan::Window");}

    //create a framebuffer for the render pass per image view
    std::vector<VkFramebuffer> fbuffs;
    fbuffs.reserve(m_imgViews.size());
    for (const auto& imgView : m_imgViews) {
        VkImageView view = reinterpret_cast<VkImageView>(imgView);
        VkFramebufferCreateInfo fbuffCreate {};
        fbuffCreate.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbuffCreate.attachmentCount = 1;
        fbuffCreate.height = res.y;
        fbuffCreate.width = res.x;
        fbuffCreate.pAttachments = &view;
        fbuffCreate.layers = 1;
        fbuffCreate.renderPass = initPass;
        VkFramebuffer fbuff;
        if (vkCreateFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), &fbuffCreate, nullptr, &fbuff) != VK_SUCCESS) 
        {throw Exception("Failed to create a dummy framebuffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Window");}
        fbuffs.push_back(fbuff);
    }

    //record the command buffer
    VkCommandBufferBeginInfo cmdBuffBeg {};
    cmdBuffBeg.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmdBuff, &cmdBuffBeg);

    //run a dummy render pass for each image
    for (const auto& fbuff : fbuffs) {
        VkClearValue clearCol {};
        clearCol.color = {{0.f, 0.f, 0.f, 0.f}};
        
        VkRenderPassBeginInfo renPassBeg {};
        renPassBeg.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renPassBeg.framebuffer = fbuff;
        renPassBeg.renderArea.offset = {0,0};
        renPassBeg.renderArea.extent = {res.x, res.y};
        renPassBeg.clearValueCount = 1;
        renPassBeg.pClearValues = &clearCol;
        renPassBeg.renderPass = initPass;
        vkCmdBeginRenderPass(cmdBuff, &renPassBeg, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdEndRenderPass(cmdBuff);
    }

    //finalize the command buffer
    vkEndCommandBuffer(cmdBuff);

    //submit to the graphics queue
    VkSubmitInfo subInfo {};
    subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    subInfo.commandBufferCount = 1;
    subInfo.pCommandBuffers = &cmdBuff;
    {
        auto queueRef = inst->getGraphicsQueue().acquire();
        vkQueueSubmit(reinterpret_cast<VkQueue>(queueRef.queue), 1, &subInfo, VK_NULL_HANDLE);
    }

    //TODO: Better
    vkQueueWaitIdle(reinterpret_cast<VkQueue>(inst->getGraphicsQueue().queues[0].first));

    //clean up
    vkDestroyCommandPool(reinterpret_cast<VkDevice>(inst->getDevice()), tempPool, nullptr);
    for (const auto& fbuff : fbuffs)
    {vkDestroyFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), fbuff, nullptr);}
    vkDestroyRenderPass(reinterpret_cast<VkDevice>(inst->getDevice()), initPass, nullptr);

    //store the new rendering resolution
    m_resolution = res;
}

bool Window::onVSyncSet(VSync vsync) {
    return false;
}