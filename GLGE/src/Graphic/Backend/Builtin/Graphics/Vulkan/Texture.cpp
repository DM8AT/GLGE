/**
 * @file Texture.cpp
 * @author DM8AT
 * @brief implement the vulkan texture backend
 * @version 0.1
 * @date 2026-05-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add textures
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Texture.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//include vulkan
#include "vulkan/vulkan.h"
//include vma
#include "vk_mem_alloc.h"

//add the shared stuff
#include "Shared.h"

GLGE::Graphic::Backend::Graphic::Vulkan::Texture::Texture(const uvec2& size, u16 mipCount, PixelFormat format, GLGE::Graphic::Backend::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::Texture(size, mipCount, format, instance)
{
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //compute the format
    VkFormat form = __pixelFormat_to_VkFormat(format);
    m_vkFormat = static_cast<i32>(form);
    //create the image
    VkImageCreateInfo imgCreate {};
    imgCreate.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgCreate.arrayLayers = 1;
    imgCreate.extent.depth = 1;
    imgCreate.extent.width = size.x;
    imgCreate.extent.height = size.y;
    imgCreate.format = form;
    imgCreate.imageType = VK_IMAGE_TYPE_2D;
    imgCreate.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreate.mipLevels = mipCount;
    imgCreate.queueFamilyIndexCount = 1;
    imgCreate.pQueueFamilyIndices = &inst->getGraphicsQueue().familyIdx;
    imgCreate.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgCreate.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgCreate.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    imgCreate.usage |= (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH || 
                        format.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL || 
                        format.order == GLGE::Graphic::PixelFormat::Order::STENCIL) 
                        ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VmaAllocationCreateInfo allocInfo {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (vmaCreateImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &imgCreate, &allocInfo, reinterpret_cast<VkImage*>(&m_image), reinterpret_cast<VmaAllocation*>(&m_allocation), nullptr)
         != VK_SUCCESS)
    {throw Exception("Failed to create an image", "GLGE::Graphic::Backend::Graphic::Vulkan::Texture::Texture");}

    //store the image aspect flags
    m_aspectFlags = static_cast<i32>(
        (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH) ? VK_IMAGE_ASPECT_DEPTH_BIT :
        (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL) ? VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT :
        (format.order == GLGE::Graphic::PixelFormat::Order::STENCIL) ? VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
    //store the layouts
    for (size_t i = 0; i < mipCount; ++i)
    {m_layout[i] = static_cast<i32>(VK_IMAGE_LAYOUT_UNDEFINED);}

    //create image view
    VkImageViewCreateInfo imgViewCreate{};
    imgViewCreate.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewCreate.format = static_cast<VkFormat>(m_vkFormat);
    imgViewCreate.image = reinterpret_cast<VkImage>(m_image);
    imgViewCreate.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    imgViewCreate.subresourceRange.baseMipLevel = 0;
    imgViewCreate.subresourceRange.levelCount = m_mips;
    imgViewCreate.subresourceRange.baseArrayLayer = 0;
    imgViewCreate.subresourceRange.layerCount = 1;
    if (vkCreateImageView(reinterpret_cast<VkDevice>(inst->getDevice()), &imgViewCreate, nullptr, reinterpret_cast<VkImageView*>(&m_view)) != VK_SUCCESS) 
    {throw Exception("Failed to create Vulkan texture view", "GLGE::Graphic::Backend::Graphic::Vulkan::Texture::Texture");}
}

GLGE::Graphic::Backend::Graphic::Vulkan::Texture::~Texture() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);
    //clean up the image
    vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(m_view), nullptr);
    vmaDestroyImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkImage>(m_image), reinterpret_cast<VmaAllocation>(m_allocation));
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Texture::update(const TextureCPU& texture) {
    //just go over all mips and update the mip levels
    for (size_t i = 0; i < texture.getMipCount(); ++i)
    {update(texture.getMip(i), i);}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Texture::update(const ImageCPU& image, u16 mip) {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //cannot transition back to layout_undefined. If it was undefined, just switch to transfer_dst_optimal
    bool wasUndefined = m_layout[mip] == static_cast<i32>(VK_IMAGE_LAYOUT_UNDEFINED);
    if (wasUndefined) 
    {m_layout[mip] = static_cast<i32>(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);}

    //transition is required if a dedicated transfer queue family exists
    bool dedicatedTransfer = inst->getGraphicsQueue().familyIdx != inst->getTransferQueue().familyIdx;
    if (dedicatedTransfer) {
        //ready the image to work on the transfer queue
        __transitionImage(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), 
                        reinterpret_cast<VkImage>(m_image), inst->getGraphicsQueue().familyIdx, inst->getTransferQueue().familyIdx, 
                        wasUndefined ? VK_IMAGE_LAYOUT_UNDEFINED : static_cast<VkImageLayout>(m_layout[mip]), static_cast<VkImageLayout>(m_layout[mip]), 
                        static_cast<VkImageAspectFlags>(m_aspectFlags), mip);
    }
    
    //get memory requirements
    u64 size = image.getSize().x*image.getSize().y*image.getTexelSize();
    //create a staging buffer
    StagingBuffer staging = __createStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), size);
    //fill in the staging data
    memcpy(staging.mapped, image.getRaw(), size);

    //get a single-time command buffer
    VkCommandBuffer cmd = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool));

    //record an image memory barrier
    //this is required because it needs to get image ownership
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = wasUndefined ? VK_IMAGE_LAYOUT_UNDEFINED : static_cast<VkImageLayout>(m_layout[mip]); //the transition is done here because it simplifies the queue family overlap logic
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = dedicatedTransfer ? inst->getGraphicsQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = dedicatedTransfer ? inst->getTransferQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = mip;
    barrier.subresourceRange.levelCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    //copy the data to the image
    VkBufferImageCopy region {};
    region.bufferRowLength = 0;
    region.bufferOffset = 0;
    region.bufferImageHeight = 0;
    region.imageExtent.depth = 1;
    region.imageExtent.width = image.getSize().x;
    region.imageExtent.height = image.getSize().y;
    region.imageOffset = {0,0,0};
    region.imageSubresource.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageSubresource.mipLevel = mip;
    vkCmdCopyBufferToImage(cmd, staging.buffer, reinterpret_cast<VkImage>(m_image), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    //record an image memory barrier
    //this is required because it needs to get image ownership
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; //the transition is done here because it simplifies the queue family overlap logic
    barrier.newLayout = static_cast<VkImageLayout>(m_layout[mip]);
    barrier.srcQueueFamilyIndex = dedicatedTransfer ? inst->getTransferQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = dedicatedTransfer ? inst->getGraphicsQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = mip;
    barrier.subresourceRange.levelCount = 1;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    //end the command buffer and submit it
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getTransferQueue(), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool), cmd);

    //get the image on the graphics queue (if graphics is a dedicated queue)
    if (dedicatedTransfer) {
        //ready the image to work on the transfer queue
        __transitionImage(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), 
                        reinterpret_cast<VkImage>(m_image), inst->getTransferQueue().familyIdx, inst->getGraphicsQueue().familyIdx, static_cast<VkImageLayout>(m_layout[mip]), static_cast<VkImageLayout>(m_layout[mip]), 
                        static_cast<VkImageAspectFlags>(m_aspectFlags), mip);
    }

    //clean up
    __destroyStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), staging);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Texture::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {

}