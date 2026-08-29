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
//add the header
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"

#include "Graphic/Instance.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//add vulkan
#include "vulkan/vulkan.h"

GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer(GLGE::Graphic::Instance* instance)
 : GLGE::Graphic::Backend::Graphic::CommandBuffer(instance)
{
    //get the device
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getInstance()->getGraphicBackendInstance().get());
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    //create the command pool
    VkCommandPoolCreateInfo cmdPoolCreate {};
    cmdPoolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreate.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolCreate.queueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    if (vkCreateCommandPool(device, &cmdPoolCreate, nullptr, reinterpret_cast<VkCommandPool*>(&m_cmdPool)) != VK_SUCCESS) 
    {throw Exception("Failed to create a command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");}

    //create one command buffer
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = reinterpret_cast<VkCommandPool>(m_cmdPool);
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_cmdBufferCount);
    if (vkAllocateCommandBuffers(device, &allocInfo, reinterpret_cast<VkCommandBuffer*>(m_cmdBuffers)) != VK_SUCCESS) {
        vkDestroyCommandPool(device, reinterpret_cast<VkCommandPool>(m_cmdPool), nullptr);
        m_cmdPool = nullptr;
        throw Exception("Failed to allocate the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::~CommandBuffer() {
    //get the device
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getInstance()->getGraphicBackendInstance().get());
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    //clean up
    if (m_cmdPool) {
        //no need to free individual command buffers, the pool deletion cleans them up too
        vkDestroyCommandPool(device, reinterpret_cast<VkCommandPool>(m_cmdPool), nullptr);
        m_cmdPool = nullptr;
        for (size_t i = 0; i < MAX_CMD_BUFFS; i++) {
            m_cmdBuffers[i] = nullptr;
        }
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::setCommandBufferCount(size_t count) {
    //count of 0 = noop
    if (count == m_cmdBufferCount || count == 0) return;
    
    //clamp
    if (count > MAX_CMD_BUFFS) 
    {count = MAX_CMD_BUFFS;}

    //get the device
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getInstance()->getGraphicBackendInstance().get());
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    //clean up
    vkFreeCommandBuffers(device, reinterpret_cast<VkCommandPool>(m_cmdPool), static_cast<uint32_t>(m_cmdBufferCount), reinterpret_cast<VkCommandBuffer*>(m_cmdBuffers));
    
    m_cmdBufferCount = count;

    //allocate the command buffer
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = reinterpret_cast<VkCommandPool>(m_cmdPool);
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_cmdBufferCount);
    if (vkAllocateCommandBuffers(device, &allocInfo, reinterpret_cast<VkCommandBuffer*>(m_cmdBuffers)) != VK_SUCCESS) 
    {throw Exception("Failed to re-allocate secondary command buffers", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::setCommandBufferCount");}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onBegin() {
    for (size_t i = 0; i < m_cmdBufferCount; i++) {
        auto* secondary = reinterpret_cast<VkCommandBuffer>(m_cmdBuffers[i]);

        //clean up old data
        if (vkResetCommandBuffer(secondary, 0) != VK_SUCCESS) 
        {throw Exception("Failed to reset the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onBegin");}

        //inheritance is required for secondaries
        VkCommandBufferInheritanceInfo inheritance{};
        inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritance.pNext = nullptr;
        inheritance.renderPass = VK_NULL_HANDLE;
        inheritance.subpass = 0;
        inheritance.framebuffer = VK_NULL_HANDLE;
        inheritance.occlusionQueryEnable = VK_FALSE;
        inheritance.queryFlags = 0;
        inheritance.pipelineStatistics = 0;
        //start the buffer
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = &inheritance;
        if (vkBeginCommandBuffer(secondary, &beginInfo) != VK_SUCCESS) 
        {throw Exception("Failed to begin the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onBegin");}
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onFinalize() {
    for (size_t i = 0; i < m_cmdBufferCount; i++) {
        auto* secondary = reinterpret_cast<VkCommandBuffer>(m_cmdBuffers[i]);

        //stop it. 
        if (vkEndCommandBuffer(secondary) != VK_SUCCESS) 
        {throw Exception("Failed to finalize the secondary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onFinalize");}
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onPlay() {
    //¯\_(ツ)_/¯
}