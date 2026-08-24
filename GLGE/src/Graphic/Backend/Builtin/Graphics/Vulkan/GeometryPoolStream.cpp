/**
 * @file GeometryPoolStream.cpp
 * @author DM8AT
 * @brief implement the geometry pool stream for vulkan
 * @version 0.1
 * @date 2026-08-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "Graphic/Backend/Builtin/Graphics/Vulkan/GeometryPoolStream.h"
//add the vulkan instance authority header
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add vulkan
#include <vulkan/vulkan.h>
//for easy vulkan memory allocation
#include "vk_mem_alloc.h"

#include <cstring>
#include <algorithm>
#include <vector>

//impl file, using namespace is ok
using namespace GLGE::Graphic::Backend::Graphic::Vulkan;

GeometryPoolStream::GeometryPoolStream(u64 size, bool isIbo, GLGE::Graphic::Backend::Graphic::Instance* instance) 
 : GeometryPool::Stream(size, isIbo, instance) {
    //Cast the generic instance to the Vulkan instance device authority
    auto* vkInst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
    if (vkInst) {
        m_device = vkInst->getDevice();
        m_allocator = vkInst->getAllocator();

        VkDevice device = static_cast<VkDevice>(m_device);
        if (device) {
            //create the synchronizing fence
            VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
            vkCreateFence(device, &fenceInfo, nullptr, reinterpret_cast<VkFence*>(&m_fence));

            //allocate upload command buffer from transfer queue pool
            VkCommandBufferAllocateInfo allocInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
            allocInfo.commandPool = static_cast<VkCommandPool>(vkInst->getTransferQueue().singleUsePool);
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;
            vkAllocateCommandBuffers(device, &allocInfo, reinterpret_cast<VkCommandBuffer*>(&m_uploadCmdBuff));
        }
    }

    //make sure to setup the size
    onResize(size);
}

GeometryPoolStream::~GeometryPoolStream() {
    //clean up the resources
    destroyResources();

    if (m_device) {
        VkDevice device = static_cast<VkDevice>(m_device);

        if (m_fence) {
            vkDestroyFence(device, static_cast<VkFence>(m_fence), nullptr);
            m_fence = nullptr;
        }

        if (m_uploadCmdBuff && m_inst) {
            auto* vkInst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
            VkCommandPool pool = static_cast<VkCommandPool>(vkInst->getTransferQueue().singleUsePool);
            VkCommandBuffer cmd = static_cast<VkCommandBuffer>(m_uploadCmdBuff);
            vkFreeCommandBuffers(device, pool, 1, &cmd);
            m_uploadCmdBuff = nullptr;
        }
    }
}

void GeometryPoolStream::waitForUpload() {
    //check that all required elements exist
    if (m_uploadInFlight && m_fence && m_device) {
        //if they are all valid, first cast the device and fence
        VkDevice device = static_cast<VkDevice>(m_device);
        VkFence fence = static_cast<VkFence>(m_fence);
        //then wait for the fence
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &fence);
        //fence finished
        m_uploadInFlight = false;
    }
}

void GeometryPoolStream::destroyResources() {
    //make sure that everything is uploaded
    waitForUpload();

    VmaAllocator allocator = static_cast<VmaAllocator>(m_allocator);
    if (!allocator) return;

    //destroy the buffer
    if (m_buffer) {
        vmaDestroyBuffer(allocator, static_cast<VkBuffer>(m_buffer), static_cast<VmaAllocation>(m_allocation));
        m_buffer = nullptr;
        m_allocation = nullptr;
    }

    //after that, delete the staging buffer
    if (m_stagingBuffer) {
        //Note: Don't manually unmap since the buffer was not manually
        vmaDestroyBuffer(allocator, static_cast<VkBuffer>(m_stagingBuffer), static_cast<VmaAllocation>(m_stagingBufferAlloc));
        m_stagingBuffer = nullptr;
        m_stagingBufferAlloc = nullptr;
    }
}

void GeometryPoolStream::onResize(u64 newSize) {
    //make sure that all uploads are done
    waitForUpload();

    VmaAllocator allocator = static_cast<VmaAllocator>(m_allocator);
    if (!allocator) return;

    //setup new main device-local buffer
    VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufInfo.size = newSize;
    bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | 
                    (m_isIBO ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    VkBuffer newBuffer = VK_NULL_HANDLE;
    VmaAllocation newAlloc = VK_NULL_HANDLE;
    vmaCreateBuffer(allocator, &bufInfo, &allocInfo, &newBuffer, &newAlloc, nullptr);

    //setup new mapped staging buffer
    VkBufferCreateInfo stagingInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    stagingInfo.size = newSize;
    stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaAllocationCreateInfo stagingAllocInfo{};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    VkBuffer newStaging = VK_NULL_HANDLE;
    VmaAllocation newStagingAlloc = VK_NULL_HANDLE;
    VmaAllocationInfo stagingResult;
    vmaCreateBuffer(allocator, &stagingInfo, &stagingAllocInfo, &newStaging, &newStagingAlloc, &stagingResult);

    //migrate existing data via GPU-to-GPU copy
    if (m_buffer && m_uploadCmdBuff && m_inst) {
        VkCommandBuffer cmd = static_cast<VkCommandBuffer>(m_uploadCmdBuff);
        
        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &beginInfo);

        VmaAllocationInfo oldAllocInfo;
        vmaGetAllocationInfo(allocator, static_cast<VmaAllocation>(m_allocation), &oldAllocInfo);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = oldAllocInfo.size;
        
        vkCmdCopyBuffer(cmd, static_cast<VkBuffer>(m_buffer), newBuffer, 1, &copyRegion);
        vkEndCommandBuffer(cmd);

        VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        auto* vkInst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
        auto queueRef = vkInst->getTransferQueue().acquire();
        vkQueueSubmit(static_cast<VkQueue>(queueRef.queue), 1, &submitInfo, static_cast<VkFence>(m_fence));
        
        m_uploadInFlight = true;
        waitForUpload();
    }

    //cleanup and reassign
    destroyResources();
    m_buffer = newBuffer;
    m_allocation = newAlloc;
    m_stagingBuffer = newStaging;
    m_stagingBufferAlloc = newStagingAlloc;
    m_mapped = stagingResult.pMappedData;
}

void GeometryPoolStream::addDirtyRange(u64 offset, u64 size) {
    //compute the end position of this dirty range
    //this makes checking for continuity easier
    u64 end = offset + size;
    //check if some regions can be merged
    for (auto& range : m_dirtyRanges) {
        u64 rangeEnd = range.offset + range.size;
        //merge contiguous or overlapping dirty ranges
        if (offset <= rangeEnd && end >= range.offset) {
            u64 newOffset = std::min(offset, range.offset);
            u64 newEnd = std::max(end, rangeEnd);
            range.offset = newOffset;
            range.size = newEnd - newOffset;
            return;
        }
    }
    //no merge possible, just add it to the end
    m_dirtyRanges.push_back({offset, size});
}

void GeometryPoolStream::onWrite(Region region, const void* data, u64 offset, u64 size) {
    //sanity check mapping
    if (!m_mapped) return;
    
    //memcopy to staging buffer
    u64 writeOffset = region.offset + offset;
    std::memcpy(static_cast<uint8_t*>(m_mapped) + writeOffset, data, size);
    
    addDirtyRange(writeOffset, size);
}

void GeometryPoolStream::onFlush() {
    //sanity check that the buffers exist
    if (m_dirtyRanges.empty() || !m_uploadCmdBuff || !m_device || !m_inst) return;

    //finish all uploads
    waitForUpload();

    //start upload cmd buff
    VkCommandBuffer cmd = static_cast<VkCommandBuffer>(m_uploadCmdBuff);
    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    //copy all dirty ranges over
    std::vector<VkBufferCopy> copies;
    copies.reserve(m_dirtyRanges.size());
    for (const auto& r : m_dirtyRanges) 
    {copies.push_back({r.offset, r.offset, r.size});}
    vkCmdCopyBuffer(cmd, static_cast<VkBuffer>(m_stagingBuffer), static_cast<VkBuffer>(m_buffer), static_cast<uint32_t>(copies.size()), copies.data());
    vkEndCommandBuffer(cmd);

    //submit work
    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    auto* vkInst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_inst);
    auto queueRef = vkInst->getTransferQueue().acquire();
    vkQueueSubmit(static_cast<VkQueue>(queueRef.queue), 1, &submitInfo, static_cast<VkFence>(m_fence));
    
    //mark that uploads are in progress, but no regions are dirty
    m_uploadInFlight = true;
    m_dirtyRanges.clear();
}