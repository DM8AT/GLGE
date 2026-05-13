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
//add vulkan shader
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Shader.h"
//add vulkan resource sets
#include "Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"
//add vulkan images
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Image.h"

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

bool dispatchCompute(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    //get the command buffers
    const std::vector<void*>& buffs = dynamic_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cmdBuff)->getBuffers();

    //extract the actual arguments
    const auto& [compute, size] = handle.getArguments<GLGE::Graphic::Shader*, GLGE::uvec3>();

    //get the vulkan objects
    auto* computeShader = dynamic_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Shader*>(compute->getBackend().get());
    VkPipeline pipe = reinterpret_cast<VkPipeline>(computeShader->getComputePipeline());
    VkPipelineLayout layout = reinterpret_cast<VkPipelineLayout>(computeShader->getComputePipelineLayout());
    VkDescriptorSet set = reinterpret_cast<VkDescriptorSet>(dynamic_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(compute->getResources(0)->getBackend().get())->getDescriptorSet());

    //iterate over all command buffers for recording
    for (const auto& buff : buffs) {
        //get the actual command buffer
        VkCommandBuffer cb = reinterpret_cast<VkCommandBuffer>(buff);

        //bind the compute pipeline
        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_COMPUTE, pipe);

        //prepare resources
        for (const auto& resource : compute->getResources(0)->resources()) {
            //depending on the type do different stuff
            switch (resource->getType())
            {
            case GLGE::Graphic::ResourceType::IMAGE: {
                    //nothing to do for images - images are always stored in general layout
                }
                break;
            
            default:
                break;
            }
        }

        //bind the descriptor set
        vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &set, 0, nullptr);

        //dispatch the compute shader
        vkCmdDispatch(cb, size.x, size.y, size.z);

        //finish the dispatch
        VkMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);
    }

    //success
    return true;
}

}