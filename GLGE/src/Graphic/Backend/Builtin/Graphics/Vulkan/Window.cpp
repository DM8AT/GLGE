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

    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getWindow()->getGraphicInstance()->getGraphicBackendInstance().get());

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
    swapCreate.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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
    
    //store the new rendering resolution
    m_resolution = res;
}

bool Window::onVSyncSet(VSync vsync) {
    return false;
}