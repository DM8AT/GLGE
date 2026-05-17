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
//add vulkan samplers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Sampler.h"
//add the resource set frontend
#include "Graphic/ResourceSet.h"

//add texture samplers
#include "Graphic/SampledTexture.h"

//include vulkan
#include "vulkan/vulkan.h"
//include vma
#include "vk_mem_alloc.h"

//add the shared stuff
#include "Shared.h"

static VkSampleCountFlagBits __clampSamples(VkSampleCountFlagBits requested, VkSampleCountFlags supported) {
    //if the supported format is supported, just return it
    if (requested & supported) return requested;

    //else, return a subset
    if (VK_SAMPLE_COUNT_8_BIT & supported) return VK_SAMPLE_COUNT_8_BIT;
    if (VK_SAMPLE_COUNT_4_BIT & supported) return VK_SAMPLE_COUNT_4_BIT;
    if (VK_SAMPLE_COUNT_2_BIT & supported) return VK_SAMPLE_COUNT_2_BIT;

    //safe fallback of 1
    return VK_SAMPLE_COUNT_1_BIT;
}

GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image(const uvec2& size, PixelFormat format, u8 samples, GLGE::Graphic::Backend::Graphic::Instance* instance) 
 : GLGE::Graphic::Backend::Graphic::Image(size, format, samples, instance)
{
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);

    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    usage |= (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH || 
              format.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL || 
              format.order == GLGE::Graphic::PixelFormat::Order::STENCIL) 
              ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_STORAGE_BIT);

    //compute the format
    VkFormat form = __pixelFormat_to_VkFormat(format);
    m_vkFormat = static_cast<i32>(form);

    //get format properties
    VkImageFormatProperties props;
    VkResult success = vkGetPhysicalDeviceImageFormatProperties(reinterpret_cast<VkPhysicalDevice>(inst->getPhysicalDevice()), static_cast<VkFormat>(m_vkFormat), 
                                                                VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usage, 0, &props);
    if (success != VK_SUCCESS)
    {throw Exception("Unsupported image format", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}

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
    imgCreate.samples = __clampSamples(__get_vulkan_sample_count(samples), props.sampleCounts);
    imgCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgCreate.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgCreate.usage = usage;
    VmaAllocationCreateInfo allocInfo {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (vmaCreateImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &imgCreate, &allocInfo, reinterpret_cast<VkImage*>(&m_image), reinterpret_cast<VmaAllocation*>(&m_allocation), nullptr)
         != VK_SUCCESS)
    {throw Exception("Failed to create an image", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}

    //store the fixed sample count
    m_samples = static_cast<u8>(static_cast<i32>(imgCreate.samples));

    //store the image aspect flags
    m_aspectFlags = static_cast<i32>(
        (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH) ? VK_IMAGE_ASPECT_DEPTH_BIT :
        (format.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL) ? VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT :
        (format.order == GLGE::Graphic::PixelFormat::Order::STENCIL) ? VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
    //store the layout
    m_layout = static_cast<i32>(VK_IMAGE_LAYOUT_UNDEFINED);

    //if MSAA was enabled, create a resolution image
    if (m_samples > 1) {
        VkImageCreateInfo resImgCreate = imgCreate;
        resImgCreate.samples = VK_SAMPLE_COUNT_1_BIT;
        if (vmaCreateImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &resImgCreate, &allocInfo, reinterpret_cast<VkImage*>(&m_img_msaaResolved), reinterpret_cast<VmaAllocation*>(&m_img_allocResolved), nullptr)
            != VK_SUCCESS)
        {throw Exception("Failed to create a resolve image", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}
    }

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
    {throw Exception("Failed to create Vulkan texture view", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}

    //create the resolved view
    if (m_samples > 1) {
        VkImageViewCreateInfo resImgViewCreate = imgViewCreate;
        resImgViewCreate.image = reinterpret_cast<VkImage>(m_img_msaaResolved);
        if (vkCreateImageView(reinterpret_cast<VkDevice>(inst->getDevice()), &resImgViewCreate, nullptr, reinterpret_cast<VkImageView*>(&m_resolvedView)) != VK_SUCCESS) 
        {throw Exception("Failed to create Vulkan texture resolve view", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}
    }

    //if this is a depth image with multi-samples, create an MSAA resolve render pass
    if ((m_aspectFlags & i32(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) && (m_samples>1)) {
        //attachment descriptions
        VkAttachmentDescription2 depthMsaa {};
        depthMsaa.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        depthMsaa.format = form;
        depthMsaa.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
        depthMsaa.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
        depthMsaa.samples = static_cast<VkSampleCountFlagBits>(m_samples);
        depthMsaa.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        depthMsaa.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthMsaa.stencilLoadOp = (static_cast<VkImageAspectFlags>(m_aspectFlags) & VK_IMAGE_ASPECT_STENCIL_BIT) ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthMsaa.stencilStoreOp = (static_cast<VkImageAspectFlags>(m_aspectFlags) & VK_IMAGE_ASPECT_STENCIL_BIT) ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
        VkAttachmentDescription2 depthResolve = depthMsaa;
        depthResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        VkAttachmentDescription2 descr[] = {depthMsaa, depthResolve};
        //attachment references
        VkAttachmentReference2 attRefMsaa {};
        attRefMsaa.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        attRefMsaa.attachment = 0;
        attRefMsaa.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
        attRefMsaa.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkAttachmentReference2 attRefResolve {};
        attRefResolve.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        attRefResolve.attachment = 1;
        attRefResolve.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
        attRefResolve.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        //resolve subpass
        VkSubpassDescriptionDepthStencilResolve dsResolve {};
        dsResolve.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE;
        dsResolve.depthResolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        dsResolve.stencilResolveMode = (attRefMsaa.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) ? VK_RESOLVE_MODE_AVERAGE_BIT : VK_RESOLVE_MODE_NONE;
        dsResolve.pDepthStencilResolveAttachment = &attRefResolve;
        VkSubpassDescription2 subpass {};
        subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pDepthStencilAttachment = &attRefMsaa;
        subpass.pNext = &dsResolve;
        //render pass
        VkRenderPassCreateInfo2 renPassCreate {};
        renPassCreate.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
        renPassCreate.attachmentCount = 2;
        renPassCreate.pAttachments = descr;
        renPassCreate.subpassCount = 1;
        renPassCreate.pSubpasses = &subpass;
        VkRenderPass pass;
        if (reinterpret_cast<PFN_vkCreateRenderPass2KHR>(inst->get_vkCreateRenderPass2KHR())(reinterpret_cast<VkDevice>(inst->getDevice()), &renPassCreate, nullptr, &pass) != VK_SUCCESS)
        {throw Exception("Failed to create a render pass to resolve depth MSAA", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}
        m_depthResolvePass = reinterpret_cast<void*>(pass);

        //create a depth buffer framebuffer
        VkImageView views[] = {reinterpret_cast<VkImageView>(m_view), reinterpret_cast<VkImageView>(m_resolvedView)};
        VkFramebufferCreateInfo fbuffCreate {};
        fbuffCreate.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbuffCreate.width = m_size.x;
        fbuffCreate.height = m_size.y;
        fbuffCreate.layers = 1;
        fbuffCreate.pAttachments = views;
        fbuffCreate.attachmentCount = 2;
        fbuffCreate.renderPass = pass;
        if (vkCreateFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), &fbuffCreate, nullptr, reinterpret_cast<VkFramebuffer*>(&m_depthFbuff)) != VK_SUCCESS)
        {throw Exception("Failed to create a depth msaa resolve framebuffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}
    }
}

GLGE::Graphic::Backend::Graphic::Vulkan::Image::~Image() {
    //get the instance
    auto* inst = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(m_instance);
    //delete the framebuffer
    vkDestroyFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkFramebuffer>(m_depthFbuff), nullptr);
    if (m_resolvedView)
    {vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(m_resolvedView), nullptr);}
    //if a resolve pass exists, delete it
    if (m_depthResolvePass)
    {vkDestroyRenderPass(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkRenderPass>(m_depthResolvePass), nullptr);}
    //clean up the image
    vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(m_view), nullptr);
    vmaDestroyImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkImage>(m_image), reinterpret_cast<VmaAllocation>(m_allocation));
    //if MSAA is enabled, destroy the resolution images
    if (m_img_msaaResolved) 
    {vmaDestroyImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkImage>(m_img_msaaResolved), reinterpret_cast<VmaAllocation>(m_img_allocResolved));}
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
        //delete the framebuffer
        vkDestroyFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkFramebuffer>(m_depthFbuff), nullptr);
        if (m_resolvedView)
        {vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(m_resolvedView), nullptr);}
        //destroy the views
        vkDestroyImageView(reinterpret_cast<VkDevice>(inst->getDevice()), reinterpret_cast<VkImageView>(m_view), nullptr);
        vmaDestroyImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkImage>(m_image), reinterpret_cast<VmaAllocation>(m_allocation));
        //if MSAA is enabled, destroy the resolution images
        if (m_img_msaaResolved) 
        {vmaDestroyImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), reinterpret_cast<VkImage>(m_img_msaaResolved), reinterpret_cast<VmaAllocation>(m_img_allocResolved));}
    }

    //store the new size
    m_size = size;
    //reset the layout (new image -> undefined)
    m_layout = static_cast<i32>(VK_IMAGE_LAYOUT_UNDEFINED);

    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    usage |= (m_format.order == GLGE::Graphic::PixelFormat::Order::DEPTH || 
              m_format.order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL || 
              m_format.order == GLGE::Graphic::PixelFormat::Order::STENCIL) 
              ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_STORAGE_BIT);

    //get format properties
    VkImageFormatProperties props;
    VkResult success = vkGetPhysicalDeviceImageFormatProperties(reinterpret_cast<VkPhysicalDevice>(inst->getPhysicalDevice()), static_cast<VkFormat>(m_vkFormat), 
                                                                VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usage, 0, &props);
    if (success != VK_SUCCESS)
    {throw Exception("Unsupported image format", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}

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
    imgCreate.samples = __clampSamples(__get_vulkan_sample_count(m_samples), props.sampleCounts);
    imgCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgCreate.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgCreate.usage = usage;
    VmaAllocationCreateInfo allocInfo {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (vmaCreateImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &imgCreate, &allocInfo, reinterpret_cast<VkImage*>(&m_image), reinterpret_cast<VmaAllocation*>(&m_allocation), nullptr)
         != VK_SUCCESS)
    {throw Exception("Failed to create an image", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}

    //if MSAA was enabled, create a resolution image
    if (m_samples > 1) {
        VkImageCreateInfo resImgCreate = imgCreate;
        resImgCreate.samples = VK_SAMPLE_COUNT_1_BIT;
        if (vmaCreateImage(reinterpret_cast<VmaAllocator>(inst->getAllocator()), &resImgCreate, &allocInfo, reinterpret_cast<VkImage*>(&m_img_msaaResolved), reinterpret_cast<VmaAllocation*>(&m_img_allocResolved), nullptr)
            != VK_SUCCESS)
        {throw Exception("Failed to create an image", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}
    }

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

    //create the resolved view
    if (m_samples > 1) {
        VkImageViewCreateInfo resImgViewCreate = imgViewCreate;
        resImgViewCreate.image = reinterpret_cast<VkImage>(m_img_msaaResolved);
        if (vkCreateImageView(reinterpret_cast<VkDevice>(inst->getDevice()), &resImgViewCreate, nullptr, reinterpret_cast<VkImageView*>(&m_resolvedView)) != VK_SUCCESS) 
        {throw Exception("Failed to create Vulkan texture resolve view", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}
    }

    if (m_depthResolvePass) {
        //create a depth buffer framebuffer
        VkImageView views[] = {reinterpret_cast<VkImageView>(m_view), reinterpret_cast<VkImageView>(m_resolvedView)};
        VkFramebufferCreateInfo fbuffCreate {};
        fbuffCreate.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbuffCreate.width = m_size.x;
        fbuffCreate.height = m_size.y;
        fbuffCreate.layers = 1;
        fbuffCreate.pAttachments = views;
        fbuffCreate.attachmentCount = 2;
        fbuffCreate.renderPass = reinterpret_cast<VkRenderPass>(m_depthResolvePass);
        if (vkCreateFramebuffer(reinterpret_cast<VkDevice>(inst->getDevice()), &fbuffCreate, nullptr, reinterpret_cast<VkFramebuffer*>(&m_depthFbuff)) != VK_SUCCESS)
        {throw Exception("Failed to create a depth msaa resolve framebuffer", "GLGE::Graphic::Backend::Graphic::Vulkan::Image::Image");}
    }

    //clear
    clear();

    //binding update required
    //iterate over all known bindings
    for (size_t i = 0; i < m_references.size(); ++i) {
        //extract the descriptor set
        auto* set = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(m_references[i].first->getBackend().get());
        VkDescriptorSet vkSet = reinterpret_cast<VkDescriptorSet>(set->getDescriptorSet());

        //get a potential sampler
        VkSampler sampler = VK_NULL_HANDLE;
        if (m_references[i].first->resources()[m_references[i].second]->getType() == GLGE::Graphic::ResourceType::SAMPLED_TEXTURE) {
            //extract the sampler
            sampler = reinterpret_cast<VkSampler>(
                        static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Sampler*>(
                            static_cast<GLGE::Graphic::SampledTexture*>(
                                (m_references[i].first->resources())[m_references[i].second]
                            )->getBackend()->getSampler().get()
                        )->getSampler()
                      );
        }
        
        //store information about the image
        VkDescriptorImageInfo info {};
        info.sampler = sampler;
        info.imageView = reinterpret_cast<VkImageView>(m_view);
        info.imageLayout = VK_IMAGE_LAYOUT_GENERAL; //this MUST be general as of the vulkan spec

        //build the write call
        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = vkSet;
        write.dstBinding = m_references[i].second;
        write.descriptorCount = 1;
        write.descriptorType = (sampler == VK_NULL_HANDLE) ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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

    //record the correct clear type for the image
    if (m_aspectFlags & VK_IMAGE_ASPECT_COLOR_BIT) {
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
        range.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
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

    //if a resolve image exists, clear it too
    if (m_img_msaaResolved) {
        //transition required
        VkImageMemoryBarrier barrier {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = static_cast<VkImageLayout>(m_layout);
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = reinterpret_cast<VkImage>(m_img_msaaResolved);
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
        if (m_aspectFlags & VK_IMAGE_ASPECT_COLOR_BIT) {
            //store the region to clear
            VkImageSubresourceRange range {};
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseArrayLayer = 0;
            range.baseMipLevel = 0;
            range.layerCount = 1;
            range.levelCount = 1;
            //color clear (always true black)
            VkClearColorValue color {};
            vkCmdClearColorImage(cmd, reinterpret_cast<VkImage>(m_img_msaaResolved), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);
        } else {
            //depth / stencil clear

            //store the region to clear
            VkImageSubresourceRange range {};
            range.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
            range.baseArrayLayer = 0;
            range.baseMipLevel = 0;
            range.layerCount = 1;
            range.levelCount = 1;
            //store the depth / stencil clear
            VkClearDepthStencilValue value {};
            vkCmdClearDepthStencilImage(cmd, reinterpret_cast<VkImage>(m_img_msaaResolved), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &range);
        }

        //transition to general required
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = reinterpret_cast<VkImage>(m_img_msaaResolved);
        barrier.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(m_aspectFlags);
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = 0;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

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