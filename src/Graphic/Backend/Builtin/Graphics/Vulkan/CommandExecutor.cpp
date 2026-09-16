/**
 * @file CommandExecutor.cpp
 * @author DM8AT
 * @brief implement the command executor for vulkan
 * @version 0.1
 * @date 2026-08-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandExecutor.h"

#include "Graphic/Window.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"
#include "Graphic/CommandStream.h"

//add vulkan
#include <vulkan/vulkan.h>
#include <cassert>

#if GLGE_DEBUG
#define CHECK_VULKAN(fun) {VkResult res = (fun); if (res != VK_SUCCESS) {std::stringstream stream; stream << #fun " : did not return VK_SUCCESS, result code: " << static_cast<i32>(res); throw GLGE::Exception(stream.str(), __ASSERT_FUNCTION);} }
#else
#define CHECK_VULKAN(fun) (fun);
#endif

#if GLGE_DEBUG && (!GLGE_NO_DB_LOG)
#define DEBUG_LOG(msg) {std::stringstream __stream; __stream << msg << "\n"; std::cout << __stream.str();}
#else
#define DEBUG_LOG(msg) 
#endif

GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::CommandExecutor(GLGE::Graphic::Window* win, GLGE::Graphic::Backend::Graphic::Instance* instance)
 : Backend::Graphic::CommandExecutor(win, instance)
{
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    //create the command pool
    VkCommandPoolCreateInfo cmdPoolCreate {};
    cmdPoolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreate.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolCreate.queueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    if (vkCreateCommandPool(device, &cmdPoolCreate, nullptr, reinterpret_cast<VkCommandPool*>(&m_cmdPool)) != VK_SUCCESS) 
    {throw Exception("Failed to create a command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::CommandExecutor");}

    DEBUG_LOG("Created new command executor owning command pool " << m_cmdPool)

    //create the command buffers
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = reinterpret_cast<VkCommandPool>(m_cmdPool);
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    if (vkAllocateCommandBuffers(device, &allocInfo, reinterpret_cast<VkCommandBuffer*>(m_cmdBuffs)) != VK_SUCCESS) {
        vkDestroyCommandPool(device, reinterpret_cast<VkCommandPool>(m_cmdPool), nullptr);
        m_cmdPool = nullptr;
        throw Exception("Failed to allocate primary command buffers", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::CommandExecutor");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {DEBUG_LOG("Created command buffer " << m_cmdBuffs[i] << " for a command executor using pool " << m_cmdBuffs << ", command buffer index: [" << i << "]")}

    VkFenceCreateInfo fenCreate {};
    fenCreate.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenCreate.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkSemaphoreCreateInfo semCreate {};
    semCreate.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    //create the fences
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(device, &fenCreate, nullptr, reinterpret_cast<VkFence*>(&(m_fences[i]))))
        {throw GLGE::Exception("Failed to create primary syncing fence", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::CommandExecutor");}
        std::cout << "Created fence " << m_fences[i] << " as a frame in flight fence\n";
        if (vkCreateSemaphore(device, &semCreate, nullptr, reinterpret_cast<VkSemaphore*>(&m_imgAvailSems[i])))
        {throw GLGE::Exception("Failed to create the image available semaphore", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::CommandExecutor");}

        DEBUG_LOG("Created frame in flight dependent fence " << m_fences[i] << " and semaphore " << m_imgAvailSems[i] << ", both belonging to frame in flight index [" << i << "]")
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::~CommandExecutor() {
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    //cannot delete during use
    awaitFinish();
    vkDeviceWaitIdle(device);

    DEBUG_LOG("Destroying command executor owning pool " << m_cmdPool)
    
    //clean the pool (this also deletes the command buffers)
    if (m_cmdPool) {
        vkDestroyCommandPool(device, reinterpret_cast<VkCommandPool>(m_cmdPool), nullptr);
        m_cmdPool = nullptr;
    }

    //clean up sync stuff
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (m_fences[i]) {vkDestroyFence(device, reinterpret_cast<VkFence>(m_fences[i]), nullptr); m_fences[i] = nullptr;}
        if (m_imgAvailSems[i]) {vkDestroySemaphore(device, reinterpret_cast<VkSemaphore>(m_imgAvailSems[i]), nullptr); m_imgAvailSems[i] = nullptr;}
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::dispatch(GLGE::Graphic::CommandStream* stream) {
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    DEBUG_LOG("Dispatching stream " << stream << " on command dispatcher using command executor " << m_cmdPool)

    //get a fence for the frame and conditionally await it
    VkFence currentFence = reinterpret_cast<VkFence>(m_fences[m_currentFrame]);
    CHECK_VULKAN(vkWaitForFences(device, 1, &currentFence, VK_TRUE, UINT64_MAX));
    CHECK_VULKAN(vkResetFences(device, 1, &currentFence));

    //select the subbuffer to use. If a window exists, that defines the window index
    u32 buff = 0;
    if (m_window) {
        VkSwapchainKHR swap = reinterpret_cast<VkSwapchainKHR>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(m_window->getGraphicWindow().get())->getSwapchain());
        CHECK_VULKAN(vkAcquireNextImageKHR(device, swap, UINT64_MAX, reinterpret_cast<VkSemaphore>(m_imgAvailSems[m_currentFrame]), VK_NULL_HANDLE, &buff));
    }

    //select the correct command buffer and clean it
    VkCommandBuffer cbuff = reinterpret_cast<VkCommandBuffer>(m_cmdBuffs[m_currentFrame]);
    if (vkResetCommandBuffer(cbuff, 0) != VK_SUCCESS)
    {throw GLGE::Exception("Failed to reset primary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::dispatch");}
    
    //then start the selected buffer
    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(cbuff, &begin) != VK_SUCCESS)
    {throw GLGE::Exception("Failed to begin primary command buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::dispatch");}

    //if a window exists, prepare the swapchain image
    if (m_window) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.image = reinterpret_cast<VkImage>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(m_window->getGraphicWindow().get())->getImages()[buff]);
        barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(cbuff, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    //then add all secondaries
    std::vector<VkCommandBuffer> buffers;
    buffers.reserve(stream->getEntries().size());
    for (const auto& cmd : stream->getEntries()) {
        if (!cmd.cmd->isDirty() && cmd.enabled) {
            auto* b = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(cmd.cmdBuff.get());
            VkCommandBuffer sec = reinterpret_cast<VkCommandBuffer>(b->getBuffer((b->getBufferCount() == 1) ? 0 : buff));
            buffers.push_back(sec);
        }
    }
    vkCmdExecuteCommands(cbuff, buffers.size(), buffers.data());

    //if a window exists, transition it back for presentation
    if (m_window) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = 0;
        barrier.image = reinterpret_cast<VkImage>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(m_window->getGraphicWindow().get())->getImages()[buff]);
        barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(cbuff, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    //finish (Yay!)
    CHECK_VULKAN(vkEndCommandBuffer(cbuff));

    //get the queue to work on
    auto q = inst->getGraphicsQueue().acquire();

    //submit to a graphic queue
    VkSemaphore waitSem[] = {reinterpret_cast<VkSemaphore>(m_imgAvailSems[m_currentFrame])};
    VkSemaphore signSem[] = {reinterpret_cast<VkSemaphore>(static_cast<Vulkan::Window*>(m_window->getGraphicWindow().get())->getSemaphores()[buff])};
    VkPipelineStageFlags waitStage[] = {VK_PIPELINE_STAGE_ALL_COMMANDS_BIT};
    VkSubmitInfo sub {};
    sub.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    sub.pWaitDstStageMask = waitStage;
    sub.commandBufferCount = 1;
    sub.pCommandBuffers = &cbuff;
    sub.waitSemaphoreCount = 1;
    sub.pWaitSemaphores = waitSem;
    sub.signalSemaphoreCount = 1;
    sub.pSignalSemaphores = signSem;
    VkResult res = vkQueueSubmit(reinterpret_cast<VkQueue>(q.queue), 1, &sub, currentFence);
    if (res != VK_SUCCESS) {
        std::stringstream stream;
        stream << "Failed to submit primary command buffer to queue. Vulkan error: " << static_cast<i32>(res);
        throw GLGE::Exception(stream.str(), "GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::dispatch");
    }
    
    //if a window exists, swap it
    if (m_window) {
        VkSwapchainKHR swap = reinterpret_cast<VkSwapchainKHR>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(m_window->getGraphicWindow().get())->getSwapchain());
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signSem;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swap;
        presentInfo.pImageIndices = &buff;
        CHECK_VULKAN(vkQueuePresentKHR(reinterpret_cast<VkQueue>(reinterpret_cast<VkQueue>(q.queue)), &presentInfo));
    }

    //update the current frame in flight
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void GLGE::Graphic::Backend::Graphic::Vulkan::CommandExecutor::awaitFinish() {
    DEBUG_LOG("Awaiting finish of command executor using pool " << m_cmdPool)
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
    VkDevice device = reinterpret_cast<VkDevice>(inst->getDevice());

    //check that all frames are finished
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkFence fen = reinterpret_cast<VkFence>(m_fences[i]);
        vkWaitForFences(device, 1, &fen, VK_TRUE, UINT64_MAX);
    }
}