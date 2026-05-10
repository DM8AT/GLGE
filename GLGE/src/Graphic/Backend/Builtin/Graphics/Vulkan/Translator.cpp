/**
 * @file Translator.cpp
 * @author DM8AT
 * @brief implement the translation functions
 * @version 0.1
 * @date 2026-05-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the translation functions
#include "Translator.h"

//add vulkan command buffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"
//add vulkan windows
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"

//add vulkan
#include "vulkan/vulkan.h"

namespace VkImpl {

bool clear(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    //extract the actual arguments
    const auto& [target, idx, color, depth, stencil] = handle.getArguments<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::vec4, GLGE::f32, GLGE::u32>();

    //get all command buffers
    const auto& cmdBuffs = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cmdBuff)->getBuffers();
    //vulkan may record multiple command buffers -> iterate over them all
    for (size_t i = 0; i < cmdBuffs.size(); ++i) {
        //extract the vulkan command buffer
        VkCommandBuffer cmdBuff = reinterpret_cast<VkCommandBuffer>(cmdBuffs[i]);

        //check the type
        if (target.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
            //TODO, framebuffers don't exist yet
        } else {
            //get the correct image
            VkImage img = reinterpret_cast<VkImage>(reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(reinterpret_cast<GLGE::Graphic::Window*>(target.getTarget())->getGraphicWindow().get())->getImages()[i]);

            //this is a swapchain image -> it must be a color attachment

            //transition to correct format
            VkImageMemoryBarrier toFormat{};
            toFormat.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            toFormat.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            toFormat.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            toFormat.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            toFormat.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            toFormat.image = img;
            toFormat.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            toFormat.subresourceRange.baseMipLevel = 0;
            toFormat.subresourceRange.levelCount = 1;
            toFormat.subresourceRange.baseArrayLayer = 0;
            toFormat.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toFormat);

            //clear the image
            VkClearColorValue clearValue = {{color.r, color.g, color.b, color.w}};
            VkImageSubresourceRange range {};
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;
            vkCmdClearColorImage(cmdBuff, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &range);

            //transition to correct intermediate format
            VkImageMemoryBarrier fromFormat{};
            fromFormat.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            fromFormat.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            fromFormat.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            fromFormat.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            fromFormat.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            fromFormat.image = img;
            fromFormat.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            fromFormat.subresourceRange.baseMipLevel = 0;
            fromFormat.subresourceRange.levelCount = 1;
            fromFormat.subresourceRange.baseArrayLayer = 0;
            fromFormat.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &fromFormat);
        }
    }

    //success
    return true;
}

}