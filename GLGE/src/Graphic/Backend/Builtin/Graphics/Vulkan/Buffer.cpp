/**
 * @file Buffer.cpp
 * @author DM8AT
 * @brief implement the vulkan buffer backend
 * @version 0.1
 * @date 2026-05-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the buffer backend
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Buffer.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add vulkan resource sets
#include "Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"
//add the resource set frontend
#include "Graphic/ResourceSet.h"

//add vulkan
#include "vulkan/vulkan.h"
//add VMA
#include "vk_mem_alloc.h"

//add shared stuff
#include "Shared.h"

//add exceptions
#include "Core/Exception.h"

/**
 * @brief a function to convert a buffer usage flag to a vulkan buffer usage flag
 * 
 * @param type the type to map
 * @return `VkBufferUsageFlags` the vulkan buffer usage flag
 */
static VkBufferUsageFlags __mapType(GLGE::Graphic::Backend::Graphic::Buffer::Type type) {
    switch (type) {
        case GLGE::Graphic::Backend::Graphic::Buffer::Type::STORAGE: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        case GLGE::Graphic::Backend::Graphic::Buffer::Type::UNIFORM: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        default: return 0;
    }
}

/**
 * @brief map the buffer usage to a vulkan memory usage
 * 
 * @param usage the memory usage flag
 * @return `VmaMemoryUsage` the vulkan memory usage
 */
static VmaMemoryUsage __mapUsage(GLGE::Graphic::Backend::Graphic::Buffer::Usage usage) {
    switch (usage) {
        case GLGE::Graphic::Backend::Graphic::Buffer::Usage::CPU_ONLY:
            return VMA_MEMORY_USAGE_CPU_ONLY;
        case GLGE::Graphic::Backend::Graphic::Buffer::Usage::GPU_ONLY:
            return VMA_MEMORY_USAGE_GPU_ONLY;
        case GLGE::Graphic::Backend::Graphic::Buffer::Usage::CPU_GPU:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        case GLGE::Graphic::Backend::Graphic::Buffer::Usage::STREAMING:
        case GLGE::Graphic::Backend::Graphic::Buffer::Usage::STREAMING_UPLOAD:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        case GLGE::Graphic::Backend::Graphic::Buffer::Usage::STREAMING_DOWNLOAD:
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        default:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::Buffer(Type type, const void* initial, size_t size, GLGE::Graphic::Backend::Graphic::Instance* instance, Usage usage) 
 : GLGE::Graphic::Backend::Graphic::Buffer(type, initial, size, instance, usage)
{
    //CPU-Only buffers don't need a GPU buffer
    if (m_usage == Usage::CPU_ONLY) {return;}

    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //write the buffer create info
    u32 queue = inst->getGraphicsQueue().familyIdx;
    VkBufferCreateInfo buffCreate {};
    buffCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffCreate.size = m_size;
    buffCreate.usage = __mapType(m_type) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffCreate.queueFamilyIndexCount = 1;
    buffCreate.pQueueFamilyIndices = &queue;
    //write the allocation info
    VmaAllocationCreateInfo allocCreate {};
    allocCreate.usage = __mapUsage(m_usage);
    if (m_usage == Usage::STREAMING || m_usage == Usage::STREAMING_UPLOAD || m_usage == Usage::STREAMING_DOWNLOAD || m_usage == Usage::CPU_GPU)
    {allocCreate.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;}
    //actually create the buffer
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
    if (vmaCreateBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &buffCreate, &allocCreate, &buffer, &allocation, &allocInfo) != VK_SUCCESS)
    {throw Exception("Failed to allocate a buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::Buffer");}
    //store the buffer and allocation
    m_buffer = reinterpret_cast<void*>(buffer);
    m_allocation = reinterpret_cast<void*>(allocation);
    //if a mapped pointer exists, use the mapped data for initialization
    if (allocInfo.pMappedData) {
        //store the mapped pointer
        m_data = allocInfo.pMappedData;
        //write the initial data
        if (initial) 
        {memcpy(m_data, initial, m_size);}
    } else if (initial) {
        //else, a staging buffer is required
        asyncUpload(initial, 0, m_size);
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::~Buffer() {
    //don't do stuff for a CPU only buffer
    if (m_usage == Usage::CPU_ONLY)
    {return;}

    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //clean up vulkan stuff
    vmaDestroyBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkBuffer>(m_buffer), reinterpret_cast<VmaAllocation>(m_allocation));
    if (m_oldBuff) 
    {vmaDestroyBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkBuffer>(m_oldBuff), reinterpret_cast<VmaAllocation>(m_oldAlloc));}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::write(const void* data, size_t size, size_t offset) {
    //if no data exists, nothing can be written
    if (!data || size == 0) {return;}

    //bounds check in debug
    #if GLGE_DEBUG
    if (offset + size > m_size) 
    {throw GLGE::Exception("Write out of bounds", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::write");}
    #endif

    //blacklist some usages
    if ((m_usage == Usage::GPU_ONLY) || (m_usage == Usage::STREAMING_DOWNLOAD)) 
    {throw GLGE::Exception("Invalid usage flag provided for CPU-Side writing", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::write");}

    //store the data
    memcpy(reinterpret_cast<u8*>(m_data) + offset, data, size);

    //mark dirty for CPU GPU shadowing system
    if (m_usage == Usage::CPU_GPU)
    {m_cpu_outOfDate.emplace_back(offset, size);}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::read(void* data, size_t size, size_t offset) {
    //sanity check the target buffer
    if (!data || size == 0) {return;}

    //bounds check in debug
    #if GLGE_DEBUG
    if (offset + size > m_size) 
    {throw GLGE::Exception("Read out of bounds", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");}
    #endif

    //switch over the usage
    switch (m_usage) {
        //CPU only and CPU GPU use the exact same read pattern
        case Usage::CPU_ONLY:
        case Usage::CPU_GPU:
            //CPU copy exists
            memcpy(data, reinterpret_cast<u8*>(m_data) + offset, size);
            break;

        //GPU only cannot be read
        case Usage::GPU_ONLY: {
                //throw an error
                throw GLGE::Exception("Cannot read from GPU_ONLY buffer", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");
            } break;

        //bidirectional streaming and stream downloading use the same download pattern
        case Usage::STREAMING:
        case Usage::STREAMING_DOWNLOAD: {
                //reading from a mapped buffer
                memcpy(data, reinterpret_cast<u8*>(m_data) + offset, size);
            } break;

        //streaming upload cannot download
        case Usage::STREAMING_UPLOAD: {
                //writing-only streaming: cannot read
                throw GLGE::Exception("Cannot read from STREAMING_UPLOAD buffer", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");
            } break;

        default:
            throw GLGE::Exception("Unknown buffer usage in read()", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");
            break;
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::resize(size_t size, bool preserve) {
    //don't do stuff for a CPU only buffer
    if (m_usage == Usage::CPU_ONLY)
    {return;}

    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //if preservation is requested, store a copy of the old data
    void* old = nullptr;
    if (preserve) {
        old = new u8[m_size];
        memcpy(old, m_data, m_size);
    }

    //create the buffer

    //write the buffer create info
    u32 queue = inst->getGraphicsQueue().familyIdx;
    VkBufferCreateInfo buffCreate {};
    buffCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffCreate.size = size;
    buffCreate.usage = __mapType(m_type) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffCreate.queueFamilyIndexCount = 1;
    buffCreate.pQueueFamilyIndices = &queue;
    //write the allocation info
    VmaAllocationCreateInfo allocCreate {};
    allocCreate.usage = __mapUsage(m_usage);
    if (m_usage == Usage::STREAMING || m_usage == Usage::STREAMING_UPLOAD || m_usage == Usage::STREAMING_DOWNLOAD || m_usage == Usage::CPU_GPU)
    {allocCreate.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;}
    //actually create the buffer
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
    if (vmaCreateBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &buffCreate, &allocCreate, &buffer, &allocation, &allocInfo) != VK_SUCCESS)
    {throw Exception("Failed to allocate a buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::Buffer");}
    //store the buffer and allocation
    if (m_oldBuff) 
    {vmaDestroyBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkBuffer>(m_oldBuff), reinterpret_cast<VmaAllocation>(m_oldAlloc));}
    m_oldBuff = m_buffer;
    m_oldAlloc = m_allocation;
    m_buffer = reinterpret_cast<void*>(buffer);
    m_allocation = reinterpret_cast<void*>(allocation);
    //if a mapped pointer exists, use the mapped data for initialization
    if (allocInfo.pMappedData) {
        //store the mapped pointer
        m_data = allocInfo.pMappedData;
        //write the initial data
        if (old) 
        {memcpy(m_data, old, m_size);}
    } else if (old) {
        //else, a staging buffer is required
        asyncUpload(old, 0, m_size);
    }

    //store information about the buffer
    VkDescriptorBufferInfo info {};
    info.offset = 0;
    info.range = size;
    info.buffer = reinterpret_cast<VkBuffer>(m_buffer);

    //trigger updates on all resource sets
    for (size_t i = 0; i < m_references.size(); ++i) {
        //build the write call
        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = reinterpret_cast<VkDescriptorSet>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(m_references[i].first->getBackend().get())->getDescriptorSet());
        write.dstBinding = m_references[i].second;
        write.descriptorCount = 1;
        write.descriptorType = (m_type == Type::STORAGE) ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &info;

        //write
        vkUpdateDescriptorSets(reinterpret_cast<VkDevice>(inst->getDevice()), 1, &write, 0, nullptr);
    }

    //store the new size
    m_size = size;
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::fill(const void* pattern, size_t patternSize, size_t n, size_t offset) {
    //quick sanity check
    if (offset > m_size) {return;}

    //set the fill size if full is selected
    if (n == 0)
    {n = (m_size - offset) / ((patternSize == 0) ? 1 : patternSize);}

    //compute total bytes to operate on, stop if 0 bytes are modified
    size_t total = patternSize * n;
    if (total == 0) {return;}

    //shrink to fit
    if (offset + total > m_size)
    {total = (m_size - offset) / ((patternSize == 0) ? 1 : patternSize);}

    //special casing for null-clear: Allowed on everything
    if (!pattern && (patternSize == 0)) {
        //clear CPU buffer
        if (m_data) 
        {memset(reinterpret_cast<u8*>(m_data) + offset, 0, total);}
        //if a GPU buffer exists and CPU to GPU streaming is disabled, clear the GPU buffer
        if ((m_usage != Usage::CPU_ONLY) && (isStreamBuffer() && (m_usage != Usage::STREAMING_DOWNLOAD))) {
            u8* dummy = new u8[m_size];
            memset(dummy, 0, m_size);
            asyncUpload(dummy, 0, m_size);
            delete[] dummy;
        }
    }

    //blacklist some usages
    if ((m_usage == Usage::GPU_ONLY) || (m_usage == Usage::STREAMING_DOWNLOAD)) 
    {throw GLGE::Exception("Invalid usage flag provided for pattern writing", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::fill");}

    //if data is stored on the CPU use CPU writes
    if (m_data) {
        //update the CPU buffer
        u8* dst = reinterpret_cast<u8*>(m_data) + offset;
        for (size_t i = 0; i < n; ++i) 
        {memcpy(dst + (i * patternSize), pattern, patternSize);}

        //for CPU shadowing mark as dirty
        if (m_usage == Usage::CPU_GPU)
        {m_cpu_outOfDate.emplace_back(offset, total);}
    }
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::sync(bool fromGPU) {
    //unused, mapped data is auto-synced
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //store the reference
    m_references.emplace_back(set, unit);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::onDropBinding(GLGE::Graphic::ResourceSet* set) {
    //remove the set
    for (size_t i = 0; i < m_references.size(); ++i)
    {if (m_references[i].first == set) {m_references.erase(m_references.begin() + i);}}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::asyncUpload(const void* data, u64 offset, u64 size) {
    //get the instance
    auto* inst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //create a staging buffer
    VkBufferCreateInfo stagingCreate {};
    stagingCreate.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingCreate.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingCreate.size = size;
    VmaAllocationCreateInfo stagingAllocCreate {};
    stagingAllocCreate.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    stagingAllocCreate.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    VkBuffer staging;
    VmaAllocation stagingAlloc;
    VmaAllocationInfo stagingInfo;
    if (vmaCreateBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &stagingCreate, &stagingAllocCreate, &staging, &stagingAlloc, &stagingInfo) != VK_SUCCESS)
    {throw Exception("Failed to create a staging buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Buffer::Buffer");}

    //load the upload data into the buffer
    memcpy(stagingInfo.pMappedData, data, size);

    //transfer queue ownership of the buffer to the transfer queue family
    //this must run on the graphics queue
    VkCommandBuffer init_transfer = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool));
    //record the barrier
    VkBufferMemoryBarrier init_barrier {};
    init_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    init_barrier.srcAccessMask = 0;
    init_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    init_barrier.srcQueueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    init_barrier.dstQueueFamilyIndex = inst->getTransferQueue().familyIdx;
    init_barrier.buffer = reinterpret_cast<VkBuffer>(m_buffer);
    init_barrier.offset = offset;
    init_barrier.size = size;
    vkCmdPipelineBarrier(init_transfer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &init_barrier, 0, nullptr);
    
    //submit to graphics
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), init_transfer);

    //start a single time command buffer to copy the data over using an async upload
    VkCommandBuffer cmdBuff = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool));

    //get buffer ownership using memory barrier
    vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &init_barrier, 0, nullptr);

    //copy data
    VkBufferCopy region {};
    region.srcOffset = 0;
    region.dstOffset = offset;
    region.size = size;
    vkCmdCopyBuffer(cmdBuff, staging, reinterpret_cast<VkBuffer>(m_buffer), 1, &region);

    //transfer ownership back to graphics
    VkBufferMemoryBarrier finalize_barrier {};
    finalize_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    finalize_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    finalize_barrier.dstAccessMask = 0;
    finalize_barrier.srcQueueFamilyIndex = inst->getTransferQueue().familyIdx;
    finalize_barrier.dstQueueFamilyIndex = inst->getGraphicsQueue().familyIdx;
    finalize_barrier.buffer = reinterpret_cast<VkBuffer>(m_buffer);
    finalize_barrier.offset = offset;
    finalize_barrier.size = size;
    vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1, &finalize_barrier, 0, nullptr);

    //submit the command buffer
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getTransferQueue(), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool), cmdBuff);

    //graphics queue must re-obtain ownership for safe future usage
    //use another single time command buffer to obtain it
    VkCommandBuffer finalize_transfer = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool));
    vkCmdPipelineBarrier(finalize_transfer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1, &finalize_barrier, 0, nullptr);
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), finalize_transfer);

    //clean up
    vmaDestroyBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), staging, stagingAlloc);
}