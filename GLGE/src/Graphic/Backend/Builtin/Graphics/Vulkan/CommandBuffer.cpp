/**
 * @file CommandBuffer.cpp
 * @author DM8AT
 * @brief implement the vulkan command buffer
 * @version 0.1
 * @date 2026-05-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the command buffer
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"

//include render pipelines
#include "Graphic/RenderPipeline.h"

//include vulkan graphic instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add windows
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"
#include "Graphic/Window.h"

//add vulkan
#include "vulkan/vulkan.h"

GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer(GLGE::Graphic::RenderPipeline* pipeline) 
 : GLGE::Graphic::Backend::Graphic::CommandBuffer(pipeline)
{
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getRenderPipeline()->getGraphicInstance()->getGraphicBackendInstance().get());
    //create the vulkan command pool
    VkCommandPoolCreateInfo cmdPoolCreate {};
    cmdPoolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreate.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolCreate.queueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    if (vkCreateCommandPool(reinterpret_cast<VkDevice>(inst->getDevice()), &cmdPoolCreate, nullptr, reinterpret_cast<VkCommandPool*>(&m_cmdPool)) != VK_SUCCESS)
    {throw Exception("Failed to create a command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");}

    //get the amount of images
    u32 imgCount = (getRenderPipeline()->getWindow())
                    ? reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(getRenderPipeline()->getWindow()->getGraphicWindow().get())->getImages().size()
                    : 1;
    m_cmdBuffers.resize(imgCount);

    //allocate a command buffers
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = reinterpret_cast<VkCommandPool>(m_cmdPool);
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = imgCount;
    if (vkAllocateCommandBuffers(reinterpret_cast<VkDevice>(inst->getDevice()), &allocInfo, reinterpret_cast<VkCommandBuffer*>(m_cmdBuffers.data())) != VK_SUCCESS)
    {throw Exception("Failed to allocate a command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");}

    //check for manual override
    if (!m_framesInFlight) {
        //compute the amount of frames in flight
        if (imgCount < 3)
        {m_framesInFlight = 2;}
        else 
        {m_framesInFlight = 3;}
    }

    //create synchronization primitives
    m_syncObjs.reserve(m_framesInFlight);
    for (size_t i = 0; i < m_framesInFlight; ++i) {
        SyncObjects syncs;
        VkSemaphoreCreateInfo imgAvailCreate {};
        imgAvailCreate.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(reinterpret_cast<VkDevice>(inst->getDevice()), &imgAvailCreate, nullptr, reinterpret_cast<VkSemaphore*>(&syncs.m_semaphore_imgAvailable)) != VK_SUCCESS)
        {throw Exception("Failed to create the image available semaphore", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");}
        VkFenceCreateInfo inFlightCreate {};
        inFlightCreate.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        inFlightCreate.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (vkCreateFence(reinterpret_cast<VkDevice>(inst->getDevice()), &inFlightCreate, nullptr, reinterpret_cast<VkFence*>(&syncs.m_fence_inFlight)) != VK_SUCCESS)
        {throw Exception("Failed to create the in flight fence", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");}
        m_syncObjs.push_back(syncs);
    }

    m_renderDones.resize(imgCount);
    for (size_t i = 0; i < imgCount; ++i) {
        VkSemaphoreCreateInfo renderDoneCreate {};
        renderDoneCreate.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(reinterpret_cast<VkDevice>(inst->getDevice()), &renderDoneCreate, nullptr, reinterpret_cast<VkSemaphore*>(&m_renderDones[i])) != VK_SUCCESS)
        {throw Exception("Failed to create the rendering done semaphore", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::CommandBuffer");}
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::~CommandBuffer() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getRenderPipeline()->getGraphicInstance()->getGraphicBackendInstance().get());

    //delete synchronization
    for (size_t i = 0; i < m_syncObjs.size(); ++i) {
        vkWaitForFences(reinterpret_cast<VkDevice>(inst->getDevice()), 1, reinterpret_cast<const VkFence*>(&m_syncObjs[i].m_fence_inFlight), VK_TRUE, UINT64_MAX);
        vkQueueWaitIdle(reinterpret_cast<VkQueue>(inst->getGraphicsQueue().queues[0].first));
        vkDestroySemaphore(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSemaphore>(m_syncObjs[i].m_semaphore_imgAvailable), nullptr);
        vkDestroyFence(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkFence>(m_syncObjs[i].m_fence_inFlight), nullptr);
    }
    for (size_t i = 0; i < m_renderDones.size(); ++i) {
        vkDestroySemaphore(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSemaphore>(m_renderDones[i]), nullptr);
    }

    //delete the command pool
    vkDestroyCommandPool(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(m_cmdPool), nullptr);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onBegin() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getRenderPipeline()->getGraphicInstance()->getGraphicBackendInstance().get());
    auto* win = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(getRenderPipeline()->getWindow()->getGraphicWindow().get());

    //wait for the old render to be done
    vkQueueWaitIdle(reinterpret_cast<VkQueue>(inst->getGraphicsQueue().queues[0].first));

    //all old images must end
    for (size_t i = 0; i < m_framesInFlight; ++i)
    {vkWaitForFences(reinterpret_cast<VkDevice>(inst->getDevice()), 1, reinterpret_cast<VkFence*>(&m_syncObjs[i].m_fence_inFlight), VK_TRUE, UINT64_MAX);}

    //begin the command buffer recording
    VkCommandBufferBeginInfo cmdBuffBeg {};
    cmdBuffBeg.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeg.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    //begin the recording for all command buffers
    for (size_t i = 0; i < m_cmdBuffers.size(); ++i) {
        vkBeginCommandBuffer(reinterpret_cast<VkCommandBuffer>(m_cmdBuffers[i]), &cmdBuffBeg);

        //switch the image from present to color attachment
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.image = reinterpret_cast<VkImage>(win->getImages()[i]);
        barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(reinterpret_cast<VkCommandBuffer>(m_cmdBuffers[i]),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
            0, 0, nullptr, 0, nullptr,
            1, &barrier
        );
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onFinalize() {
    auto* win = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(getRenderPipeline()->getWindow()->getGraphicWindow().get());
    //end the command buffer recording
    for (size_t i = 0; i < m_cmdBuffers.size(); ++i) {
        //switch the image from color attachment to present
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = 0;
        barrier.image = reinterpret_cast<VkImage>(win->getImages()[i]);
        barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(reinterpret_cast<VkCommandBuffer>(m_cmdBuffers[i]),
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr,
            1, &barrier
        );

        vkEndCommandBuffer(reinterpret_cast<VkCommandBuffer>(m_cmdBuffers[i]));
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer::onPlay() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(getRenderPipeline()->getGraphicInstance()->getGraphicBackendInstance().get());

    //todo: Better
    vkWaitForFences(reinterpret_cast<VkDevice>(inst->getDevice()), 1, reinterpret_cast<const VkFence*>(&m_syncObjs[m_frameIdx].m_fence_inFlight), VK_TRUE, UINT64_MAX);
    vkResetFences(reinterpret_cast<VkDevice>(inst->getDevice()), 1, reinterpret_cast<const VkFence*>(&m_syncObjs[m_frameIdx].m_fence_inFlight));

    //if a window is bound, pull the correct buffer, else just use buffer 0
    u32 buff = 0;
    if (getRenderPipeline()->getWindow()) {
        auto* win = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(getRenderPipeline()->getWindow()->getGraphicWindow().get());
        vkAcquireNextImageKHR(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkSwapchainKHR>(win->getSwapchain()), UINT64_MAX, 
                              reinterpret_cast<VkSemaphore>(m_syncObjs[m_frameIdx].m_semaphore_imgAvailable), VK_NULL_HANDLE, &buff);
    }

    //store the command buffer
    VkCommandBuffer cmdBuff = reinterpret_cast<VkCommandBuffer>(m_cmdBuffers[buff]);

    VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //submit the command buffer
    VkSubmitInfo subInfo {};
    subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    subInfo.commandBufferCount = 1;
    subInfo.pCommandBuffers = &cmdBuff;
    subInfo.waitSemaphoreCount = 1;
    subInfo.pWaitSemaphores = reinterpret_cast<VkSemaphore*>(&m_syncObjs[m_frameIdx].m_semaphore_imgAvailable);
    subInfo.signalSemaphoreCount = 1;
    subInfo.pSignalSemaphores = reinterpret_cast<VkSemaphore*>(&m_renderDones[buff]);
    subInfo.pWaitDstStageMask = &waitStages;
    //submit to the queue
    {
        auto queueRef = inst->getGraphicsQueue().acquire();
        vkQueueSubmit(reinterpret_cast<VkQueue>(queueRef.queue), 1, &subInfo, reinterpret_cast<VkFence>(m_syncObjs[m_frameIdx].m_fence_inFlight));
    }

    //if a window exists, present
    if (getRenderPipeline()->getWindow()) {
        auto* win = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(getRenderPipeline()->getWindow()->getGraphicWindow().get());
        auto swapchain = reinterpret_cast<VkSwapchainKHR>(win->getSwapchain());

        VkPresentInfoKHR present {};
        present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = reinterpret_cast<VkSemaphore*>(&m_renderDones[buff]);
        present.swapchainCount = 1;
        present.pSwapchains = &swapchain;
        present.pImageIndices = &buff;

        {
            auto queueRef = inst->getGraphicsQueue().acquire();
            vkQueuePresentKHR(reinterpret_cast<VkQueue>(queueRef.queue), &present);
        }
    }

    //step the frame
    m_frameIdx = (m_frameIdx + 1) % m_syncObjs.size();
}