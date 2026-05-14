/**
 * @file Image.cpp
 * @author DM8AT
 * @brief implement the image for vulkan
 * @version 0.1
 * @date 2026-05-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add vulkan images
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Image.h"
//add vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add vulkan resource sets
#include "Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"
//add the resource set frontend
#include "Graphic/ResourceSet.h"

//include vulkan
#include "vulkan/vulkan.h"
//include vma
#include "vk_mem_alloc.h"

//add the shared stuff
#include "Shared.h"

GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image(const uvec2& size, PixelFormat format, u8 samples, GLGE::Graphic::Backend::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::Image(size, format, samples, instance)
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
    imgCreate.mipLevels = 1;
    imgCreate.queueFamilyIndexCount = 1;
    imgCreate.pQueueFamilyIndices = &inst->getGraphicsQueue().familyIdx;
    imgCreate.samples = static_cast<VkSampleCountFlagBits>(glm::min(static_cast<i32>(__get_vulkan_sample_count(m_samples)), inst->getMaxSampleCount()));
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
    {throw Exception("Failed to create an image", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}

    //store the image aspect flags
    m_aspectFlags = static_cast<i32>(
        (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH) ? VK_IMAGE_ASPECT_DEPTH_BIT :
        (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL) ? VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT :
        (format.order == GLGE::Graphic::PixelFormat::Order::STENCIL) ? VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
    //store the layout
    m_layout = static_cast<i32>(VK_IMAGE_LAYOUT_UNDEFINED);

    //create image view
    VkImageViewCreateInfo imgViewCreate{};
    imgViewCreate.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewCreate.format = static_cast<VkFormat>(m_vkFormat);
    imgViewCreate.image = reinterpret_cast<VkImage>(m_image);
    imgViewCreate.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    imgViewCreate.subresourceRange.baseMipLevel = 0;
    imgViewCreate.subresourceRange.levelCount = 1;
    imgViewCreate.subresourceRange.baseArrayLayer = 0;
    imgViewCreate.subresourceRange.layerCount = 1;
    if (vkCreateImageView(reinterpret_cast<VkDevice>(inst->getDevice()), &imgViewCreate, nullptr, reinterpret_cast<VkImageView*>(&m_view)) != VK_SUCCESS) 
    {throw Exception("Failed to create Vulkan texture view", "GLGE::Graphic::Backend::Graphic::Vulkan::Texture::Texture");}
}

GLGE::Graphic::Backend::Graphic::Vulkan::Image::~Image() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);
    //clean up the image
    vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(m_view), nullptr);
    vmaDestroyImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkImage>(m_image), reinterpret_cast<VmaAllocation>(m_allocation));
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::upload(const ImageCPU& image) {
    //just use write with a position of 0,0
    write(image, {0,0});
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::write(const ImageCPU& image, const uvec2& position) {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //cannot transition back to layout_undefined. If it was undefined, just switch to general
    bool wasUndefined = m_layout == static_cast<i32>(VK_IMAGE_LAYOUT_UNDEFINED);
    if (wasUndefined) 
    {m_layout = static_cast<i32>(VK_IMAGE_LAYOUT_GENERAL);}

    //transition is required if a dedicated transfer queue family exists
    bool dedicatedTransfer = inst->getGraphicsQueue().familyIdx != inst->getTransferQueue().familyIdx;
    if (dedicatedTransfer) {
        //ready the image to work on the transfer queue
        __transitionImage(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), 
                        reinterpret_cast<VkImage>(m_image), inst->getGraphicsQueue().familyIdx, inst->getTransferQueue().familyIdx, 
                        wasUndefined ? VK_IMAGE_LAYOUT_UNDEFINED : static_cast<VkImageLayout>(m_layout), static_cast<VkImageLayout>(m_layout), 
                        static_cast<VkImageAspectFlags>(m_aspectFlags), 0);
    }
    
    //get memory requirements
    u64 size = image.getSize().x*image.getSize().y*image.getTexelSize();
    //create a staging buffer
    StagingBuffer staging = __createStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), size, inst->getGraphicsQueue().familyIdx);
    //fill in the staging data
    memcpy(staging.mapped, image.getRaw(), size);

    //get a single-time command buffer
    VkCommandBuffer cmd = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool));

    //record an image memory barrier
    //this is required because it needs to get image ownership
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = wasUndefined ? VK_IMAGE_LAYOUT_UNDEFINED : static_cast<VkImageLayout>(m_layout); //the transition is done here because it simplifies the queue family overlap logic
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = dedicatedTransfer ? inst->getGraphicsQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = dedicatedTransfer ? inst->getTransferQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
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
    region.imageOffset.x = position.x;
    region.imageOffset.y = position.y;
    region.imageOffset.z = 0;
    region.imageSubresource.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageSubresource.mipLevel = 0;
    vkCmdCopyBufferToImage(cmd, staging.buffer, reinterpret_cast<VkImage>(m_image), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    //record an image memory barrier
    //this is required because it needs to get image ownership
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; //the transition is done here because it simplifies the queue family overlap logic
    barrier.newLayout = static_cast<VkImageLayout>(m_layout);
    barrier.srcQueueFamilyIndex = dedicatedTransfer ? inst->getTransferQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = dedicatedTransfer ? inst->getGraphicsQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
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
                        reinterpret_cast<VkImage>(m_image), inst->getTransferQueue().familyIdx, inst->getGraphicsQueue().familyIdx, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<VkImageLayout>(m_layout), 
                        static_cast<VkImageAspectFlags>(m_aspectFlags), 0);
    }

    //clean up
    __destroyStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), staging);

    //the image is now guaranteed to be in general layout
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::resizeAndClear(const uvec2& size) {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //clean up
    if (m_image) {
        vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(m_view), nullptr);
        vmaDestroyImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkImage>(m_image), reinterpret_cast<VmaAllocation>(m_allocation));
    }

    //store the new size
    m_size = size;
    //reset the layout (new image -> undefined)
    m_layout = static_cast<i32>(VK_IMAGE_LAYOUT_UNDEFINED);

    //re-create the image (similar to constructor)
    //create the image
    VkImageCreateInfo imgCreate {};
    imgCreate.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgCreate.arrayLayers = 1;
    imgCreate.extent.depth = 1;
    imgCreate.extent.width = size.x;
    imgCreate.extent.height = size.y;
    imgCreate.format = static_cast<VkFormat>(m_vkFormat);
    imgCreate.imageType = VK_IMAGE_TYPE_2D;
    imgCreate.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreate.mipLevels = 1;
    imgCreate.queueFamilyIndexCount = 1;
    imgCreate.pQueueFamilyIndices = &inst->getGraphicsQueue().familyIdx;
    imgCreate.samples = static_cast<VkSampleCountFlagBits>(glm::min(static_cast<i32>(__get_vulkan_sample_count(m_samples)), inst->getMaxSampleCount()));
    imgCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgCreate.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgCreate.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    imgCreate.usage |= (m_aspectFlags | VK_IMAGE_ASPECT_COLOR_BIT) ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VmaAllocationCreateInfo allocInfo {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (vmaCreateImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &imgCreate, &allocInfo, reinterpret_cast<VkImage*>(&m_image), reinterpret_cast<VmaAllocation*>(&m_allocation), nullptr)
         != VK_SUCCESS)
    {throw Exception("Failed to create an image", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}

    //create image view
    VkImageViewCreateInfo imgViewCreate{};
    imgViewCreate.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewCreate.format = static_cast<VkFormat>(m_vkFormat);
    imgViewCreate.image = reinterpret_cast<VkImage>(m_image);
    imgViewCreate.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    imgViewCreate.subresourceRange.baseMipLevel = 0;
    imgViewCreate.subresourceRange.levelCount = 1;
    imgViewCreate.subresourceRange.baseArrayLayer = 0;
    imgViewCreate.subresourceRange.layerCount = 1;
    if (vkCreateImageView(reinterpret_cast<VkDevice>(inst->getDevice()), &imgViewCreate, nullptr, reinterpret_cast<VkImageView*>(&m_view)) != VK_SUCCESS) 
    {throw Exception("Failed to create Vulkan texture view", "GLGE::Graphic::Backend::Graphic::Vulkan::Texture::Texture");}

    //clear
    clear();

    //binding update required
    //iterate over all known bindings
    for (size_t i = 0; i < m_references.size(); ++i) {
        //extract the descriptor set
        auto* set = dynamic_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(m_references[i].first->getBackend().get());
        VkDescriptorSet vkSet = reinterpret_cast<VkDescriptorSet>(set->getDescriptorSet());
        
        //store information about the image
        VkDescriptorImageInfo info {};
        info.sampler = VK_NULL_HANDLE;
        info.imageView = reinterpret_cast<VkImageView>(m_view);
        info.imageLayout = VK_IMAGE_LAYOUT_GENERAL; //this MUST be general as of the vulkan spec

        //build the write call
        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = vkSet;
        write.dstBinding = m_references[i].second;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        write.pImageInfo = &info;

        //update the descriptor set
        vkUpdateDescriptorSets(reinterpret_cast<VkDevice>(inst->getDevice()), 1, &write, 0, nullptr);
    }

    //framebuffer update required
    for (size_t i = 0; i < m_ref_framebuffers.size(); ++i) 
    {m_ref_framebuffers[i]->onImageUpdated(this);}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::read(ImageCPU& out) const {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //transition is required if a dedicated transfer queue family exists
    bool dedicatedTransfer = inst->getGraphicsQueue().familyIdx != inst->getTransferQueue().familyIdx;
    if (dedicatedTransfer) {
        //ready the image to work on the transfer queue
        __transitionImage(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), 
                        reinterpret_cast<VkImage>(m_image), inst->getGraphicsQueue().familyIdx, inst->getTransferQueue().familyIdx, 
                        static_cast<VkImageLayout>(m_layout), static_cast<VkImageLayout>(m_layout), 
                        static_cast<VkImageAspectFlags>(m_aspectFlags), 0);
    }
    
    //compute the byte size
    u64 size = m_size.x*m_size.y*((m_format.r_Bitcount + m_format.g_Bitcount + m_format.b_Bitcount + m_format.a_Bitcount + 7) / 8);
    //allocate a staging buffer
    StagingBuffer staging = __createStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), size, inst->getGraphicsQueue().familyIdx);
    //get a single-time command buffer
    VkCommandBuffer cmd = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool));

    //record an image memory barrier
    //this is required because it needs to get image ownership
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = static_cast<VkImageLayout>(m_layout); //the transition is done here because it simplifies the queue family overlap logic
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = dedicatedTransfer ? inst->getGraphicsQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = dedicatedTransfer ? inst->getTransferQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    //copy the data to the image
    VkBufferImageCopy region {};
    region.bufferRowLength = 0;
    region.bufferOffset = 0;
    region.bufferImageHeight = 0;
    region.imageExtent.depth = 1;
    region.imageExtent.width = m_size.x;
    region.imageExtent.height = m_size.y;
    region.imageSubresource.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageSubresource.mipLevel = 0;
    vkCmdCopyImageToBuffer(cmd, reinterpret_cast<VkImage>(m_image), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging.buffer, 1, &region);

    //record an image memory barrier
    //this is required because it needs to get image ownership
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; //the transition is done here because it simplifies the queue family overlap logic
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcQueueFamilyIndex = dedicatedTransfer ? inst->getTransferQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = dedicatedTransfer ? inst->getGraphicsQueue().familyIdx : VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    //end the command buffer and submit it
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getTransferQueue(), reinterpret_cast<VkCommandPool>(inst->getTransferQueue().singleUsePool), cmd);

    //get the image on the graphics queue (if graphics is a dedicated queue)
    if (dedicatedTransfer) {
        //ready the image to work on the transfer queue
        __transitionImage(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), 
                        reinterpret_cast<VkImage>(m_image), inst->getTransferQueue().familyIdx, inst->getGraphicsQueue().familyIdx, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 
                        static_cast<VkImageAspectFlags>(m_aspectFlags), 0);
    }

    //prepare the CPU image
    out = GLGE::Graphic::ImageCPU(staging.mapped, m_format, m_size);

    //clean up
    __destroyStagingBuffer(reinterpret_cast<VmaAllocator>(inst->getAllocator()), staging);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::clear() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    //begin a single use command buffer
    VkCommandBuffer cmd = __beginSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool));

    //transition required
    VkImageMemoryBarrier barrier {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = static_cast<VkImageLayout>(m_layout);
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    //it is now in transfer_dst_optimal
    m_layout = static_cast<i32>(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    //record the correct clear type for the image
    if (m_aspectFlags | VK_IMAGE_ASPECT_COLOR_BIT) {
        //store the region to clear
        VkImageSubresourceRange range {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseArrayLayer = 0;
        range.baseMipLevel = 0;
        range.layerCount = 1;
        range.levelCount = 1;
        //color clear (always true black)
        VkClearColorValue color {};
        vkCmdClearColorImage(cmd, reinterpret_cast<VkImage>(m_image), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);
    } else {
        //depth / stencil clear

        //store the region to clear
        VkImageSubresourceRange range {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseArrayLayer = 0;
        range.baseMipLevel = 0;
        range.layerCount = 1;
        range.levelCount = 1;
        //store the depth / stencil clear
        VkClearDepthStencilValue value {};
        vkCmdClearDepthStencilImage(cmd, reinterpret_cast<VkImage>(m_image), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &range);
    }

    //transition to general required
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = reinterpret_cast<VkImage>(m_image);
    barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = 0;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    //submit the command buffer
    __endSingleTimeCommands(reinterpret_cast<VkDevice>(inst->getDevice()), inst->getGraphicsQueue(), reinterpret_cast<VkCommandPool>(inst->getGraphicsQueue().singleUsePool), cmd);

    //the image is now guaranteed to be in general
    m_layout = static_cast<i32>(VK_IMAGE_LAYOUT_GENERAL);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //register the binding
    m_references.emplace_back(set, unit);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::onDropBinding(GLGE::Graphic::ResourceSet* set) {
    for (size_t i = 0; i < m_references.size(); ++i)
    {if (m_references[i].first == set) {m_references.erase(m_references.begin() + i);}}
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::registerFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) {
    //store the framebuffer binding
    m_ref_framebuffers.push_back(fbuff);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Image::removeFramebuffer(GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff) {
    for (size_t i = 0; i < m_ref_framebuffers.size(); ++i)
    {if (m_ref_framebuffers[i] == fbuff) {m_ref_framebuffers.erase(m_ref_framebuffers.begin() + i);}}
}