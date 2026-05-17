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
//add vulkan framebuffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Framebuffer.h"

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
            //get the framebuffer
            auto* fbuff = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer*>(static_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend().get());
            //iterate over all color attachments
            for (size_t i = 0; i < fbuff->getColorAttachmentCount(); ++i) {
                //extract the image
                auto* img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(fbuff->getColorAttachment(i));
                VkImage vkImg = reinterpret_cast<VkImage>(img->getImage());

                //clear the image
                VkClearColorValue clearValue = {{color.r, color.g, color.b, color.w}};
                VkImageSubresourceRange range {};
                range.aspectMask = static_cast<VkImageAspectFlags>(img->getAspectFlags());
                range.baseMipLevel = 0;
                range.levelCount = 1;
                range.baseArrayLayer = 0;
                range.layerCount = 1;
                vkCmdClearColorImage(cmdBuff, vkImg, static_cast<VkImageLayout>(img->getLayout()), &clearValue, 1, &range);
            }
            //iterate over all depth attachments
            for (size_t i = 0; i < fbuff->getDepthAttachmentCount(); ++i) {
                //extract the image
                auto* img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(fbuff->getDepthAttachment(i));
                VkImage vkImg = reinterpret_cast<VkImage>(img->getImage());

                //clear the image
                VkClearDepthStencilValue ds {};
                ds.depth = depth;
                ds.stencil = stencil;
                VkImageSubresourceRange range {};
                range.aspectMask = static_cast<VkImageAspectFlags>(img->getAspectFlags());
                range.baseMipLevel = 0;
                range.levelCount = 1;
                range.baseArrayLayer = 0;
                range.layerCount = 1;
                vkCmdClearDepthStencilImage(cmdBuff, vkImg, static_cast<VkImageLayout>(img->getLayout()), &ds, 1, &range);
            }
        } else {
            //get the correct image
            VkImage img = reinterpret_cast<VkImage>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(static_cast<GLGE::Graphic::Window*>(target.getTarget())->getGraphicWindow().get())->getImages()[i]);

            //this is a swapchain image -> it must be a color attachment

            //clear the image
            VkClearColorValue clearValue = {{color.r, color.g, color.b, color.w}};
            VkImageSubresourceRange range {};
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;
            vkCmdClearColorImage(cmdBuff, img, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &range);
        }
    }

    //success
    return true;
}

bool copy(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    //get the command buffers
    const std::vector<void*>& buffs = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cmdBuff)->getBuffers();

    //extract the actual arguments
    const auto& [from, from_idx, to, to_idx, copyDepth, copyStencil] = handle.getArguments<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::Graphic::RenderTarget, GLGE::u8, bool, bool>();
    
    //iterate over all buffers
    for (size_t i = 0; i < buffs.size(); ++i) {
        //get the command buffer
        VkCommandBuffer cb = reinterpret_cast<VkCommandBuffer>(buffs[i]);

        //get the vulkan objects
        struct ImgInfo {
            VkImage image = VK_NULL_HANDLE;
            VkImage resImage = VK_NULL_HANDLE;
            GLGE::uvec2 size = {0,0};
            VkImageLayout layout;
            VkImageAspectFlags aspects;
            VkSampleCountFlagBits samples;

            ImgInfo(GLGE::Graphic::RenderTarget target, GLGE::u8 idx, GLGE::u8 winIdx, bool depthStencil) {
                if (target.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                    //use a window
                    auto* win = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(reinterpret_cast<GLGE::Graphic::Window*>(target.getTarget())->getGraphicWindow().get());
                    image = reinterpret_cast<VkImage>(win->getImages()[winIdx]);
                    size = win->getResolution();
                    layout = VK_IMAGE_LAYOUT_GENERAL;
                    aspects = VK_IMAGE_ASPECT_COLOR_BIT;
                    samples = VK_SAMPLE_COUNT_1_BIT;
                } else {
                    //use a framebuffer
                    GLGE::Graphic::Backend::Graphic::Vulkan::Image* img = nullptr;
                    if (depthStencil) {
                        img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend()->getDepthAttachment(0));
                    } else {
                        img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend()->getColorAttachment(idx));
                    }
                    image = reinterpret_cast<VkImage>(img->getImage());
                    resImage = reinterpret_cast<VkImage>(img->getResolveImage());
                    size = img->getSize();
                    layout = VK_IMAGE_LAYOUT_GENERAL;
                    aspects = static_cast<VkImageAspectFlags>(img->getAspectFlags());
                    samples = static_cast<VkSampleCountFlagBits>(img->getSamplesPerPixel());
                }
            }

            ImgInfo() = default;
        };
        ImgInfo fromInfo(from, from_idx, i, false);
        ImgInfo fromDepth;
        if (copyDepth || copyStencil) {fromDepth = ImgInfo(from, from_idx, i, true);}
        ImgInfo toInfo(to, to_idx, i, false);
        ImgInfo toDepth;
        if (copyDepth || copyStencil) {toDepth = ImgInfo(to, to_idx, i, true);}

        //sanity checks for debug
        #if GLGE_DEBUG
        if (fromInfo.aspects != toInfo.aspects)
        {throw GLGE::Exception("Tried to copy data between incompatible image aspects", "GLGE::Graphic::Backend::Graphic::Vulkan::Translators::copy");}
        if (toInfo.samples > 1)
        {throw GLGE::Exception("Tried to blit to a multi-sample image", "GLGE::Graphic::Backend::Graphic::Vulkan::Translators::copy");}
        #endif

        //make sure the image is currently up to date
        VkImageMemoryBarrier barrier {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = fromInfo.image;
        barrier.oldLayout = fromInfo.layout;
        barrier.newLayout = fromInfo.layout;
        barrier.subresourceRange.aspectMask = fromInfo.aspects;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        //multi-sample resolving
        if (fromInfo.samples > 1) {
            //TODO: add depth resolution
            if (!(fromInfo.aspects & VK_IMAGE_ASPECT_COLOR_BIT))
            {throw GLGE::Exception("Only color images can currently be resolved from MSAA to single images. Please contact the maintainer.", "GLGE::Graphic::Backend::Graphic::Vulkan::Translators::copy");}

            //resolve the image
            VkImageResolve resolve {};
            resolve.srcSubresource = {fromInfo.aspects, 0, 0, 1};
            resolve.srcOffset = {0,0,0};
            resolve.dstSubresource = {fromInfo.aspects, 0, 0, 1};
            resolve.dstOffset = {0,0,0};
            resolve.extent = {fromInfo.size.x, fromInfo.size.y, 1};
            vkCmdResolveImage(cb, fromInfo.image, fromInfo.layout, fromInfo.resImage, VK_IMAGE_LAYOUT_UNDEFINED, 1, &resolve);
            //replace the copy image with the resolved image
            fromInfo.image = fromInfo.resImage;
        }

        //use blit to copy from one image to another
        VkImageBlit reg {};
        reg.srcOffsets[0].x = 0;
        reg.srcOffsets[1].x = fromInfo.size.x;
        reg.srcOffsets[0].y = 0;
        reg.srcOffsets[1].y = fromInfo.size.y;
        reg.srcOffsets[0].z = 0;
        reg.srcOffsets[1].z = 1;
        reg.srcSubresource.aspectMask = fromInfo.aspects;
        reg.srcSubresource.baseArrayLayer = 0;
        reg.srcSubresource.mipLevel = 0;
        reg.srcSubresource.layerCount = 1;
        reg.dstOffsets[0].x = 0;
        reg.dstOffsets[1].x = toInfo.size.x;
        reg.dstOffsets[0].y = 0;
        reg.dstOffsets[1].y = toInfo.size.y;
        reg.dstOffsets[0].z = 0;
        reg.dstOffsets[1].z = 1;
        reg.dstSubresource.aspectMask = toInfo.aspects;
        reg.dstSubresource.baseArrayLayer = 0;
        reg.dstSubresource.mipLevel = 0;
        reg.dstSubresource.layerCount = 1;
        vkCmdBlitImage(cb, fromInfo.image, fromInfo.layout, toInfo.image, toInfo.layout, 1, &reg, VK_FILTER_NEAREST);

        //if depth data exists, do the same for the depth
        if (fromDepth.image != VK_NULL_HANDLE && toDepth.image != VK_NULL_HANDLE) {
            VkImageBlit reg {};
            reg.srcOffsets[0].x = 0;
            reg.srcOffsets[1].x = fromDepth.size.x;
            reg.srcOffsets[0].y = 0;
            reg.srcOffsets[1].y = fromDepth.size.y;
            reg.srcOffsets[0].z = 0;
            reg.srcOffsets[1].z = 1;
            reg.srcSubresource.aspectMask = fromDepth.aspects;
            reg.srcSubresource.baseArrayLayer = 0;
            reg.srcSubresource.mipLevel = 0;
            reg.srcSubresource.layerCount = 1;
            reg.dstOffsets[0].x = 0;
            reg.dstOffsets[1].x = toDepth.size.x;
            reg.dstOffsets[0].y = 0;
            reg.dstOffsets[1].y = toDepth.size.y;
            reg.dstOffsets[0].z = 0;
            reg.dstOffsets[1].z = 1;
            reg.dstSubresource.aspectMask = toDepth.aspects;
            reg.dstSubresource.baseArrayLayer = 0;
            reg.dstSubresource.mipLevel = 0;
            reg.dstSubresource.layerCount = 1;
            vkCmdBlitImage(cb, fromDepth.image, fromDepth.layout, toDepth.image, toDepth.layout, 1, &reg, VK_FILTER_NEAREST);
        }
    }

    //success
    return true;
}

bool dispatchCompute(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    //get the command buffers
    const std::vector<void*>& buffs = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cmdBuff)->getBuffers();

    //extract the actual arguments
    const auto& [compute, size] = handle.getArguments<GLGE::Graphic::Shader*, GLGE::uvec3>();

    //get the vulkan objects
    auto* computeShader = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Shader*>(compute->getBackend().get());
    VkPipeline pipe = reinterpret_cast<VkPipeline>(computeShader->getComputePipeline());
    VkPipelineLayout layout = reinterpret_cast<VkPipelineLayout>(computeShader->getComputePipelineLayout());
    VkDescriptorSet set = reinterpret_cast<VkDescriptorSet>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(compute->getResources(0)->getBackend().get())->getDescriptorSet());

    //iterate over all command buffers for recording
    for (const auto& buff : buffs) {
        //get the actual command buffer
        VkCommandBuffer cb = reinterpret_cast<VkCommandBuffer>(buff);

        //prepare the dispatch
        VkMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);

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
        vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);
    }

    //success
    return true;
}

bool drawWorld(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::Vulkan::Translators::drawWorld");

    //extract all arguments
    const auto& [renderer] = handle.getArguments<GLGE::Graphic::Renderer*>();

    //draw
    renderer->getBackend()->record(cmdBuff);
    
    //success
    return true;
}

}