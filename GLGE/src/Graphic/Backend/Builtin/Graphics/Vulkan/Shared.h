/**
 * @file Shared.h
 * @author DM8AT
 * @brief define some shared functions for vulkan
 * @version 0.1
 * @date 2026-05-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#pragma once

//add vulkan
#include "vulkan/vulkan.h"
//add VMA
#include "vk_mem_alloc.h"

//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//add pixel formats
#include "Graphic/ColorInfo.h"
//add exceptions
#include "Core/Exception.h"

inline static VkFormat __pixelFormat_to_VkFormat(const GLGE::Graphic::PixelFormat& format) {
    //check for depth formats
    if (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH) {
        switch (format.r_Bitcount) {
            case 16: return VK_FORMAT_D16_UNORM; break;
            case 24: return VK_FORMAT_X8_D24_UNORM_PACK32;
            case 32: {
                    if (format.type == GLGE::Graphic::PixelFormat::Type::FLOAT) {
                        return VK_FORMAT_D32_SFLOAT;
                    } else {
                        //error
                        throw GLGE::Exception("Failed to convert pixel format to vulkan format - depth format type error", "GLGE::Graphic::Backend::Graphic::Vulkan::Internal::__pixelFormat_to_VkFormat");
                        return VK_FORMAT_UNDEFINED;
                    }
                }
                break;
        
        default:
            //error
            throw GLGE::Exception("Failed to convert pixel format to vulkan format - depth format bit count not supported", "GLGE::Graphic::Backend::Graphic::Vulkan::Internal::__pixelFormat_to_VkFormat");
            return VK_FORMAT_UNDEFINED;
            break;
        }
    }

    //check for depth stencil formats
    if (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL) {
        //just special case everything
        if (format.r_Bitcount == 24 && format.g_Bitcount == 8)
        {return VK_FORMAT_D24_UNORM_S8_UINT;}
        if (format.r_Bitcount == 32 && format.g_Bitcount == 8 && format.type == GLGE::Graphic::PixelFormat::Type::FLOAT)
        {return VK_FORMAT_D32_SFLOAT_S8_UINT;}
        //error
        throw GLGE::Exception("Failed to convert pixel format to vulkan format - depth stencil format not supported", "GLGE::Graphic::Backend::Graphic::Vulkan::Internal::__pixelFormat_to_VkFormat");
        return VK_FORMAT_UNDEFINED;
    }

    //check for stencil formats
    if (format.order == GLGE::Graphic::PixelFormat::Order::STENCIL) {
        //only 8 bit supported
        if (format.r_Bitcount == 8)
        {return VK_FORMAT_S8_UINT;}
        //error
        throw GLGE::Exception("Failed to convert pixel format to vulkan format - stencil format not supported", "GLGE::Graphic::Backend::Graphic::Vulkan::Internal::__pixelFormat_to_VkFormat");
        return VK_FORMAT_UNDEFINED;
    }

    //for color format: Too much variety, just use lookup tables
    struct Entry {
        GLGE::Graphic::PixelFormat::Order order;
        GLGE::Graphic::PixelFormat::Type  type;
        std::array<GLGE::u8, 4> bc;  // r,g,b,a
        VkFormat vk;
    };

    //Thanks ChatGPT for generating this LUT
    static const Entry LUT[] = {
        // --- R ---
        { GLGE::Graphic::PixelFormat::Order::R, GLGE::Graphic::PixelFormat::Type::UNORM, std::array<GLGE::u8, 4>{8,0,0,0}, VK_FORMAT_R8_UNORM },
        { GLGE::Graphic::PixelFormat::Order::R, GLGE::Graphic::PixelFormat::Type::UINT,  std::array<GLGE::u8, 4>{8,0,0,0}, VK_FORMAT_R8_UINT  },
        { GLGE::Graphic::PixelFormat::Order::R, GLGE::Graphic::PixelFormat::Type::FLOAT, std::array<GLGE::u8, 4>{32,0,0,0},VK_FORMAT_R32_SFLOAT },

        // --- RG ---
        { GLGE::Graphic::PixelFormat::Order::RG, GLGE::Graphic::PixelFormat::Type::UNORM, std::array<GLGE::u8, 4>{8,8,0,0}, VK_FORMAT_R8G8_UNORM },
        { GLGE::Graphic::PixelFormat::Order::RG, GLGE::Graphic::PixelFormat::Type::UINT,  std::array<GLGE::u8, 4>{8,8,0,0}, VK_FORMAT_R8G8_UINT  },
        { GLGE::Graphic::PixelFormat::Order::RG, GLGE::Graphic::PixelFormat::Type::FLOAT, std::array<GLGE::u8, 4>{32,32,0,0},VK_FORMAT_R32G32_SFLOAT },

        // --- RGB ---
        { GLGE::Graphic::PixelFormat::Order::RGB, GLGE::Graphic::PixelFormat::Type::UNORM, std::array<GLGE::u8, 4>{8,8,8,0}, VK_FORMAT_R8G8B8_UNORM },
        { GLGE::Graphic::PixelFormat::Order::RGB, GLGE::Graphic::PixelFormat::Type::UINT,  std::array<GLGE::u8, 4>{8,8,8,0}, VK_FORMAT_R8G8B8_UINT  },
        { GLGE::Graphic::PixelFormat::Order::RGB, GLGE::Graphic::PixelFormat::Type::FLOAT, std::array<GLGE::u8, 4>{32,32,32,0},VK_FORMAT_R32G32B32_SFLOAT },

        // --- BGR ---
        { GLGE::Graphic::PixelFormat::Order::BGR, GLGE::Graphic::PixelFormat::Type::UNORM, std::array<GLGE::u8, 4>{8,8,8,0}, VK_FORMAT_B8G8R8_UNORM },
        { GLGE::Graphic::PixelFormat::Order::BGR, GLGE::Graphic::PixelFormat::Type::UINT,  std::array<GLGE::u8, 4>{8,8,8,0}, VK_FORMAT_B8G8R8_UINT  },

        // --- RGBA ---
        { GLGE::Graphic::PixelFormat::Order::RGBA, GLGE::Graphic::PixelFormat::Type::UNORM, std::array<GLGE::u8, 4>{8,8,8,8}, VK_FORMAT_R8G8B8A8_UNORM },
        { GLGE::Graphic::PixelFormat::Order::RGBA, GLGE::Graphic::PixelFormat::Type::UINT,  std::array<GLGE::u8, 4>{8,8,8,8}, VK_FORMAT_R8G8B8A8_UINT },
        { GLGE::Graphic::PixelFormat::Order::RGBA, GLGE::Graphic::PixelFormat::Type::FLOAT, std::array<GLGE::u8, 4>{16,16,16,16}, VK_FORMAT_R16G16B16A16_SFLOAT},
        { GLGE::Graphic::PixelFormat::Order::RGBA, GLGE::Graphic::PixelFormat::Type::FLOAT, std::array<GLGE::u8, 4>{32,32,32,32}, VK_FORMAT_R32G32B32A32_SFLOAT },

        // --- BGRA ---
        { GLGE::Graphic::PixelFormat::Order::BGRA, GLGE::Graphic::PixelFormat::Type::UNORM, std::array<GLGE::u8, 4>{8,8,8,8}, VK_FORMAT_B8G8R8A8_UNORM },
        { GLGE::Graphic::PixelFormat::Order::BGRA, GLGE::Graphic::PixelFormat::Type::UINT,  std::array<GLGE::u8, 4>{8,8,8,8}, VK_FORMAT_B8G8R8A8_UINT }

        // --- ARGB (requires packed formats if desired) ---
        // Extend as necessary.

        // --- INDEXED ---
        // Vulkan doesn't support paletted formats directly; return undefined.
    };

    //pack the bit count into an array
    const std::array<GLGE::u8, 4> bc {
        format.r_Bitcount,
        format.g_Bitcount,
        format.b_Bitcount,
        format.a_Bitcount
    };

    //look for a fitting format
    for (const auto& element : LUT) {
        if (element.order == format.order && element.type == format.type && element.bc == bc) {
            //successful match
            return element.vk;
        }
    }
    //error: format not supported
    throw GLGE::Exception("Failed to convert pixel format to vulkan format - color format not supported", "GLGE::Graphic::Backend::Graphic::Vulkan::Internal::__pixelFormat_to_VkFormat");
    return VK_FORMAT_UNDEFINED;
}

inline static VkSampleCountFlagBits __get_vulkan_sample_count(GLGE::u8 samples) {
    //early out for one
    if (samples <= 1) return VK_SAMPLE_COUNT_1_BIT;
    //round up to closest power of two
    uint32_t v = 1;
    while (v < samples && v < 64) 
    {v <<= 1;}
    //return closest vulkan fit
    switch (v) {
        case 1:  return VK_SAMPLE_COUNT_1_BIT;
        case 2:  return VK_SAMPLE_COUNT_2_BIT;
        case 4:  return VK_SAMPLE_COUNT_4_BIT;
        case 8:  return VK_SAMPLE_COUNT_8_BIT;
        case 16: return VK_SAMPLE_COUNT_16_BIT;
        case 32: return VK_SAMPLE_COUNT_32_BIT;
        default: return VK_SAMPLE_COUNT_64_BIT;
    }
}

/**
 * @brief begin a single time command buffer
 * 
 * @param device the device to use for the command buffer
 * @param pool the command pool to allocate from
 * @return `VkCommandBuffer` the allocated and ready command buffer
 */
inline static VkCommandBuffer __beginSingleTimeCommands(VkDevice device, VkCommandPool pool) {
    //allocate the command buffer from the inputted pool
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;
    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(device, &allocInfo, &cmd);

    //start the command buffer recording
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    //give back the command buffer
    return cmd;
}

/**
 * @brief end a single time command buffer
 * 
 * @param device the device the buffer belongs to
 * @param queue the queue pool to pull the queue from
 * @param pool the command pool the buffer was allocated from
 * @param cmd the actual command buffer
 */
inline static void __endSingleTimeCommands(VkDevice device, GLGE::Graphic::Backend::Graphic::Vulkan::Instance::QueuePool& queue, VkCommandPool pool, VkCommandBuffer cmd) {
    //end the recording
    vkEndCommandBuffer(cmd);

    //prepare submission
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    //submit and wait for the queue
    //the sync object is only needed DURING submission, NOT execution
    VkQueue q;
    {
        auto queueRef = queue.acquire();
        q = reinterpret_cast<VkQueue>(queueRef.queue);
        vkQueueSubmit(q, 1, &submitInfo, VK_NULL_HANDLE);
    }
    vkQueueWaitIdle(q);

    //free the command buffer
    vkFreeCommandBuffers(device, pool, 1, &cmd);
}

/**
 * @brief store information about a staging buffer
 */
struct StagingBuffer {
    /**
     * @brief store the actual buffer
     */
    VkBuffer buffer = VK_NULL_HANDLE;
    /**
     * @brief store the actual allocation
     */
    VmaAllocation allocation = nullptr;
    /**
     * @brief store a pointer to the mapped data
     */
    void* mapped = nullptr;
};

/**
 * @brief create a staging buffer
 * 
 * @param allocator the allocator to use for allocations
 * @param size the size of the buffer
 * @param queue the queue family index the buffer will belong to
 * @return `StagingBuffer` the filled, ready-to-use, staging buffer
 */
inline static StagingBuffer __createStagingBuffer(VmaAllocator allocator, VkDeviceSize size, GLGE::u32 queue) {
    //store the staging buffer to return
    StagingBuffer buff{};

    //create the buffer creation info
    VkBufferCreateInfo buffInfo {};
    buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffInfo.size = size;
    buffInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffInfo.queueFamilyIndexCount = 1;
    buffInfo.pQueueFamilyIndices = &queue;
    //create the allocation info
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    //store the allocation result
    VmaAllocationInfo allocRes {};
    if (vmaCreateBuffer(allocator, &buffInfo, &allocInfo, &buff.buffer, &buff.allocation, &allocRes) != VK_SUCCESS)
    {throw GLGE::Exception("Failed to create a staging buffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Internal::__createStagingBuffer");}
    //store the mapped pointer
    buff.mapped = allocRes.pMappedData;

    //return the buffer
    return buff;
}

/**
 * @brief delete a staging buffer
 * 
 * @param allocator the allocator the buffer was allocated from
 * @param buffer the staging buffer to delete
 */
inline static void __destroyStagingBuffer(VmaAllocator allocator, StagingBuffer& buffer) {
    //sanity check the input
    if (buffer.buffer == VK_NULL_HANDLE) {return;}
    //destroy the buffer
    vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
    //clean up
    buffer.buffer = VK_NULL_HANDLE;
    buffer.allocation = nullptr;
    buffer.mapped = nullptr;
}

/**
 * @brief transition an image from one layout to another
 * 
 * @param device the vulkan device
 * @param queuePool the queue pool to work on
 * @param pool the pool 
 * @param image the image to work on
 * @param oldQueue the old queue (or `VK_QUEUE_FAMILY_IGNORED`) to transition away from
 * @param newQueue the new queue (or `VK_QUEUE_FAMILY_IGNORED`) to transition to
 * @param oldLayout the old layout of the image
 * @param newLayout the new layout of the image
 * @param aspect the image aspect flags
 * @param mip the index of the mip level to use
 */
inline static void __transitionImage(VkDevice device, GLGE::Graphic::Backend::Graphic::Vulkan::Instance::QueuePool& queuePool, VkCommandPool pool, VkImage image, GLGE::u32 oldQueue, GLGE::u32 newQueue, 
                       VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspect, GLGE::u16 mip) {
    //get a single-time command buffer
    VkCommandBuffer cmd = __beginSingleTimeCommands(device, pool);

    //record an image memory barrier
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = oldQueue;
    barrier.dstQueueFamilyIndex = newQueue;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = mip;
    barrier.subresourceRange.levelCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    //end the command buffer and submit it
    __endSingleTimeCommands(device, queuePool, pool, cmd);
}