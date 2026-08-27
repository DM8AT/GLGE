/**
 * @file CommandBuffer.cpp
 * @author DM8AT
 * @brief Implement the Vulkan secondary command buffer
 * @version 0.1
 * @date 2026-05-08
 *
 * @copyright Copyright (c) 2026
 *
 */

//include the command buffer
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"

//include instances
#include "Graphic/Instance.h"

//include vulkan graphic instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//add vulkan
#include "vulkan/vulkan.h"


GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::
CommandBuffer(GLGE::Graphic::Instance* instance)
    : GLGE::Graphic::Backend::Graphic::CommandBuffer(instance)
{
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getInstance()->getGraphicBackendInstance().get());

    //get the device
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());
    //create the vulkan command pool
    VkCommandPoolCreateInfo cmdPoolCreate {};
    cmdPoolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreate.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolCreate.queueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    if (vkCreateCommandPool(device, &cmdPoolCreate, nullptr, reinterpret_cast<VkCommandPool*>(&m_cmdPool)) != VK_SUCCESS) 
    {throw Exception("Failed to create a command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");}

    //allocate a single secondary command buffer
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = reinterpret_cast<VkCommandPool>(m_cmdPool);
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(device, &allocInfo, reinterpret_cast<VkCommandBuffer*>(&m_cmdBuffer)) != VK_SUCCESS) {
        vkDestroyCommandPool(device, reinterpret_cast<VkCommandPool>(m_cmdPool), nullptr);
        m_cmdPool = nullptr;
        throw Exception("Failed to allocate the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::~CommandBuffer() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getInstance()->getGraphicBackendInstance().get());
    //get the device
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    //The command buffer itself does not need to be explicitly freed because destroying its command pool also frees all command buffers allocated from it.
    if (m_cmdPool) {
        vkDestroyCommandPool(device, reinterpret_cast<VkCommandPool>(m_cmdPool), nullptr);
        m_cmdPool = nullptr;
        m_cmdBuffer = nullptr;
    }
}


void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onBegin(GLGE::Graphic::Window* window) {
    //ignore the window
    (void)window;
    auto* secondary = reinterpret_cast<VkCommandBuffer>(m_cmdBuffer);

    //reset the command buffer
    if (vkResetCommandBuffer(secondary, 0) != VK_SUCCESS) 
    {throw Exception("Failed to reset the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onBegin");}

    //just start the recording
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(secondary, &beginInfo) != VK_SUCCESS) 
    {throw Exception("Failed to begin the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onBegin");}
}


void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onEnd(GLGE::Graphic::Window* window) {
    //NOOP
    (void)window;
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onFinalize() {
    auto* secondary = reinterpret_cast<VkCommandBuffer>(m_cmdBuffer);

    if (vkEndCommandBuffer(secondary) != VK_SUCCESS) 
    {throw Exception("Failed to finalize the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onFinalize");}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onPlay() {
    //¯\_(ツ)_/¯
}