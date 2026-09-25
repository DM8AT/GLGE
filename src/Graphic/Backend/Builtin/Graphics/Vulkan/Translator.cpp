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
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"
//add vulkan windows
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"
//add vulkan shader
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/Shader.h"
//add vulkan resource sets
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"
//add vulkan images
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/Image.h"
//add vulkan framebuffers
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/Framebuffer.h"
//add vulkan instances
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add vulkan buffers
#include "GLGE/Graphic/Backend/Builtin/Graphics/Vulkan/Buffer.h"

//add vulkan
#include "vulkan/vulkan.h"

/**
 * @brief a helper function to translate a depth compare mode
 * 
 * @param mode the mode to translate
 * @return `VkCompareOp` the translation result
 */
inline static VkCompareOp __toVkCompareOp(GLGE::Graphic::Backend::Graphic::Material::DepthMode mode) {
    switch (mode) {
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_ALWAYS: return VK_COMPARE_OP_ALWAYS; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_NEVER: return VK_COMPARE_OP_NEVER; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS: return VK_COMPARE_OP_LESS; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS_EQUALS: return VK_COMPARE_OP_LESS_OR_EQUAL; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_GREATER: return VK_COMPARE_OP_GREATER; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_GREATER_EQUALS: return VK_COMPARE_OP_GREATER_OR_EQUAL; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_EQUALS: return VK_COMPARE_OP_EQUAL; break;
        case GLGE::Graphic::Backend::Graphic::Material::DepthMode::DEPTH_COMPARE_NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL; break;

        default: return VK_COMPARE_OP_ALWAYS;
    }
}

/**
 * @brief a helper to translate the cull mode
 * 
 * @param mode the cull mode to translate
 * @return `VkCullModeFlags` the translation result
 */
inline static VkCullModeFlags __toVkCullMode(GLGE::Graphic::Backend::Graphic::Material::CullMode mode) {
    switch (mode) {
        case GLGE::Graphic::Backend::Graphic::Material::CullMode::OFF: return VK_CULL_MODE_NONE; break;
        case GLGE::Graphic::Backend::Graphic::Material::CullMode::BACK: return VK_CULL_MODE_BACK_BIT; break;
        case GLGE::Graphic::Backend::Graphic::Material::CullMode::FRONT: return VK_CULL_MODE_FRONT_BIT; break;

        default: return VK_CULL_MODE_NONE;
    }
}

namespace VkImpl {

bool clear(GLGE::Graphic::Backend::Graphic::CommandBuffer& cBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    //extract the actual arguments
    const auto& [target, idx, color, depth, stencil] = handle.getArguments<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::vec4, GLGE::f32, GLGE::u32>();

    //if the target is a window, update the command buffer size
    if (target.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
        //a single command buffer per swap-chain image is required
        size_t count = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(reinterpret_cast<GLGE::Graphic::Window*>(target.getTarget())->getGraphicWindow().get())->getImages().size();
        static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cBuff)->setCommandBufferCount(count);
        //restart of cmd buff is required
        cBuff.onBegin();
    }

    //vulkan may record multiple command buffers -> iterate over them all
    for (size_t i = 0; i < static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cBuff)->getBufferCount(); ++i) {
        //extract the vulkan command buffer
        VkCommandBuffer cmdBuff = reinterpret_cast<VkCommandBuffer>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cBuff)->getBuffer(i));

        //check the type
        if (target.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
            //get the framebuffer
            auto* fbuff = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer*>(static_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend().get());
            //iterate over all color attachments
            for (size_t i = 0; i < fbuff->getColorAttachmentCount(); ++i) {
                //extract the image
                auto* img = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(fbuff->getColorAttachment(i));
                VkImage vkImg = reinterpret_cast<VkImage>(img->getImage());

                //syncing
                VkImageMemoryBarrier barrier {};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
                barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = vkImg;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

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

                //syncing
                VkImageMemoryBarrier barrier {};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
                barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = vkImg;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | (fbuff->usesDepthStencil() ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

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

            //syncing
            VkImageMemoryBarrier barrier {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = img;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(cmdBuff, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

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

bool copy(GLGE::Graphic::Backend::Graphic::CommandBuffer& cBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    //extract the actual arguments
    const auto& [from, from_idx, to, to_idx] = handle.getArguments<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::Graphic::RenderTarget, GLGE::u8>();
    
    //if the target is a window, update the command buffer size
    if (from.getType() == GLGE::Graphic::RenderTarget::WINDOW || to.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
        //a single command buffer per swap-chain image is required
        size_t count = 0;
        if (from.getType() == GLGE::Graphic::RenderTarget::WINDOW) {count = std::max<size_t>(count, static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(reinterpret_cast<GLGE::Graphic::Window*>(from.getTarget())->getGraphicWindow().get())->getImages().size());}
        if (to.getType()   == GLGE::Graphic::RenderTarget::WINDOW) {count = std::max<size_t>(count, static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(reinterpret_cast<GLGE::Graphic::Window*>(to.getTarget())->getGraphicWindow().get())->getImages().size());}
        static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cBuff)->setCommandBufferCount(count);
        //restart of cmd buff is required
        cBuff.onBegin();
    }

    //iterate over all buffers
    for (size_t i = 0; i < static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cBuff)->getBufferCount(); ++i) {
        //get the command buffer
        VkCommandBuffer cb = reinterpret_cast<VkCommandBuffer>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cBuff)->getBuffer(i));

        //get the vulkan objects
        struct ImgInfo {
            VkImage image = VK_NULL_HANDLE;
            VkImage resImage = VK_NULL_HANDLE;
            VkFramebuffer fbuff = VK_NULL_HANDLE;
            VkRenderPass pass = VK_NULL_HANDLE;
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
                    fbuff = reinterpret_cast<VkFramebuffer>(img->getDepthFramebuffer());
                    pass = reinterpret_cast<VkRenderPass>(img->getDepthResolveRenderPass());
                }
            }

            ImgInfo() = default;
        };
        ImgInfo fromInfo(from, from_idx, i, false);
        ImgInfo toInfo(to, to_idx, i, false);

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
            //resolve the image
            VkImageResolve resolve {};
            resolve.srcSubresource = {fromInfo.aspects, 0, 0, 1};
            resolve.srcOffset = {0,0,0};
            resolve.dstSubresource = {fromInfo.aspects, 0, 0, 1};
            resolve.dstOffset = {0,0,0};
            resolve.extent = {fromInfo.size.x, fromInfo.size.y, 1};
            vkCmdResolveImage(cb, fromInfo.image, fromInfo.layout, fromInfo.resImage, VK_IMAGE_LAYOUT_GENERAL, 1, &resolve);
            //replace the copy image with the resolved image
            fromInfo.image = fromInfo.resImage;

            //re-validate
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
    }

    //success
    return true;
}

bool dispatchCompute(GLGE::Graphic::Backend::Graphic::CommandBuffer& cBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    //get the command buffer
    VkCommandBuffer cb = reinterpret_cast<VkCommandBuffer>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cBuff)->getBuffer(0));

    //extract the actual arguments
    const auto& [compute, size] = handle.getArguments<GLGE::Graphic::Shader*, GLGE::uvec3>();

    //get the vulkan objects
    auto* computeShader = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Shader*>(compute->getBackend().get());
    VkPipeline pipe = reinterpret_cast<VkPipeline>(computeShader->getComputePipeline());
    VkPipelineLayout layout = reinterpret_cast<VkPipelineLayout>(computeShader->getComputePipelineLayout());
    VkDescriptorSet set = reinterpret_cast<VkDescriptorSet>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(compute->getResources(0)->getBackend().get())->getDescriptorSet());

    //prepare the dispatch
    VkMemoryBarrier barrierInit {};
    barrierInit.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrierInit.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barrierInit.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1, &barrierInit, 0, nullptr, 0, nullptr);

    //bind the compute pipeline
    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_COMPUTE, pipe);

    //bind the descriptor set
    vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &set, 0, nullptr);

    //dispatch the compute shader
    vkCmdDispatch(cb, size.x, size.y, size.z);

    //prepare the dispatch
    VkMemoryBarrier barrierFinalize {};
    barrierFinalize.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrierFinalize.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    barrierFinalize.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &barrierFinalize, 0, nullptr, 0, nullptr);

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

bool drawDebug(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::Vulkan::Translators::drawDebug");

    //extract all arguments
    const auto& [context] = handle.getArguments<GLGE::Graphic::DebugContext*>();

    //get the vulkan instance
    const auto* vkInst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(context->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());
    VkDevice device = reinterpret_cast<VkDevice>(vkInst->getDevice());

    //store persistent data
    struct Persistent {
        /**
         * @brief structure to store pipeline keys
         */
        struct PipelineKey {
            /**
             * @brief the used shader
             */
            GLGE::Graphic::Shader* shader;
            /**
             * @brief the target to render to
             */
            GLGE::Graphic::RenderTarget target;
            /**
             * @brief For some reason this cannot be set with dynamic state without using extended_dynamic_state3, which has poor device coverage
             */
            GLGE::Graphic::DebugDrawDataProvider::Style::BlendMode blendMode;
            /**
             * @brief store the render mode used for the pipeline
             */
            GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode renderMode;

            /**
             * @brief compare two pipeline keys
             * 
             * @return `true` if they are identical, `false` if not
             */
            constexpr bool operator==(const PipelineKey&) const noexcept = default;

            /**
             * @brief functor to hash a pipeline key
             */
            struct Hasher {
                std::size_t operator()(const PipelineKey& toHash) const noexcept {
                    //store the seed
                    std::size_t seed = std::hash<GLGE::Graphic::Shader*>{}(toHash.shader);

                    //similar to boost::hash_combine
                    auto hashCombine = [&seed](std::size_t value) 
                                       {seed ^= value + static_cast<std::size_t>(0x9e3779b9) + (seed << 6) + (seed >> 2);};
                    //special function to hash enums
                    auto hashEnum = [&hashCombine](auto value) {
                        using T = std::remove_cv_t<decltype(value)>;
                        using U = std::underlying_type_t<T>;
                        hashCombine(std::hash<U>{}(static_cast<U>(value)));
                    };

                    //add the blend mode to it
                    hashEnum(toHash.blendMode);
                    hashEnum(toHash.renderMode);

                    //add the render target to it
                    hashCombine(std::hash<GLGE::Graphic::RenderTarget>{}(toHash.target));

                    //return the seed
                    return seed;
                }
            };
        };

        /**
         * @brief store a pipeline
         */
        struct PipelineStorage {
            /**
             * @brief store the actual vulkan pipeline
             */
            VkPipeline pipeline;
            /**
             * @brief store the vulkan pipeline layout
             */
            VkPipelineLayout layout;
            /**
             * @brief store the generation of the element
             * 
             * Overflow detection is required, stepping up from 0xff to 0x00 is valid
             */
            GLGE::u8 generation;

            /**
             * @brief check if two pipelines are identical
             * 
             * @return `true` if they are identical, `false` if they are not identical
             */
            constexpr bool operator==(const PipelineStorage&) const noexcept = default;
        };

        /**
         * @brief store the current generation
         */
        GLGE::u8 generation = 0;
        /**
         * @brief store all pipelines
         */
        std::unordered_map<PipelineKey, PipelineStorage, PipelineKey::Hasher> pipelines;
    };

    //clean up the old persistent data if it exists
    if (context->getBackendData() == nullptr) {
        context->setBackendData(new Persistent {});
        context->setCleanupFn([](GLGE::Graphic::DebugContext* context) -> void {
            //get the vulkan instance
            const auto* vkInst = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Instance*>(context->getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get());
            VkDevice device = reinterpret_cast<VkDevice>(vkInst->getDevice());

            //get persistent data
            Persistent* data = reinterpret_cast<Persistent*>(context->getBackendData());
            for (const auto& [_, pipeline] : data->pipelines) {
                vkDestroyPipelineLayout(device, pipeline.layout, nullptr);
                vkDestroyPipeline(device, pipeline.pipeline, nullptr);
            }
        });
    }
    //get the persistent data
    Persistent* persistent = reinterpret_cast<Persistent*>(context->getBackendData());

    //store the current generation
    GLGE::u8 currentGen = persistent->generation + 1;
    //pre-scan to create all pipelines
    GLGE::Graphic::Shader* currentShader = nullptr;
    GLGE::Graphic::RenderTarget currentTarget = static_cast<GLGE::Graphic::Window*>(nullptr);
    //store a list of all camera matrices
    std::vector<glm::mat4> camMatrices;
    camMatrices.reserve(context->getCameraBuffer()->getSize() / sizeof(glm::mat4));
    //store a list of all target extents
    std::vector<GLGE::uvec2> extents;
    extents.reserve(context->getTargetInfoBuffer()->getSize() / sizeof(GLGE::uvec2));
    for (const auto& cmd : context->getCommands()) {
        //scan for shader setting
        if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetShader>(cmd.command)) {
            //just store that this is the shader
            currentShader = std::get<GLGE::Graphic::DebugContext::Command::SetShader>(cmd.command).shader;
        } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetCamera>(cmd.command)) {
            //get the camera
            const auto& camera = std::get<GLGE::Graphic::DebugContext::Command::SetCamera>(cmd.command).camera;
            const auto& pos = std::get<GLGE::Graphic::DebugContext::Command::SetCamera>(cmd.command).pos;
            //compute the projection matrix
            glm::mat4 proj = glm::perspectiveZO(glm::radians(camera.FOV), 1.f, camera.clip_near, camera.clip_far);
            proj[1][1] *= -1.f;
            //compute the transformation matrix
            glm::mat4 transf = glm::translate(glm::mat4(GLGE::Quaternion(camera.eulerAngles)), -pos);
            //combine into one matrix
            glm::mat4 camMatrix = proj * transf;

            //store in the list
            camMatrices.push_back(camMatrix);
        } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetTarget>(cmd.command)) {
            //just store that this is the target
            currentTarget = std::get<GLGE::Graphic::DebugContext::Command::SetTarget>(cmd.command).target;

            //store the extent of the target
            GLGE::uvec2 extent;
            if (currentTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                extent = reinterpret_cast<GLGE::Graphic::Window*>(currentTarget.getTarget())->getResolution();
            } else if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                extent = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize();
            } else {
                std::unreachable();
            }
            extents.push_back(extent);
        } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::Draw>(cmd.command)) {
            //get the draw cmd
            const auto& drawCmd = std::get<GLGE::Graphic::DebugContext::Command::Draw>(cmd.command);
            //iterate over all sub-commands
            for (size_t i = drawCmd.cmdStartIdx; i < (drawCmd.cmdStartIdx + drawCmd.drawCount); ++i) {
                //create a pipeline key to check if the required pipeline exists
                auto style = context->getDrawSubCommands()[i].style;
                Persistent::PipelineKey key {
                    .shader = currentShader,
                    .target = currentTarget,
                    .blendMode = style.blendMode,
                    .renderMode = style.renderMode
                };

                //check if the pipeline exists
                auto it = persistent->pipelines.find(key);
                if (it != persistent->pipelines.end()) {
                    //make sure to update the generation
                    it->second.generation = currentGen;
                    continue;
                }

                //pipeline requirements not met -> create the pipeline

                //create the pipeline layout
                auto* vkShader = reinterpret_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Shader*>(currentShader->getBackend().get());
                VkPipelineLayoutCreateInfo pipeLayoutCreate {};
                pipeLayoutCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipeLayoutCreate.setLayoutCount = vkShader->getLayouts().size();
                pipeLayoutCreate.pSetLayouts = reinterpret_cast<VkDescriptorSetLayout const*>(vkShader->getLayouts().data());
                VkPipelineLayout vkLayout;
                if (vkCreatePipelineLayout(device, &pipeLayoutCreate, nullptr, &vkLayout) != VK_SUCCESS)
                {throw GLGE::Exception("Failed to create a pipeline layout", "GLGE::Graphic::Backend::Graphic::Vulkan::Translators::drawDebug");}

                //define the dynamic states
                const VkDynamicState dynamicStates[] = {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR,

                    VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT,
                    VK_DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT,

                    VK_DYNAMIC_STATE_CULL_MODE_EXT
                };

                //setup the input descriptions
                VkVertexInputBindingDescription inputDescr {};
                inputDescr.binding = 0;
                inputDescr.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                inputDescr.stride = sizeof(GLGE::vec3);

                VkVertexInputAttributeDescription attrDescr {};
                attrDescr.binding = 0;
                attrDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
                attrDescr.location = 0;
                attrDescr.offset = 0;

                //create the vertex input layout
                VkPipelineVertexInputStateCreateInfo vertInputCreate {};
                vertInputCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertInputCreate.vertexBindingDescriptionCount = 1;
                vertInputCreate.pVertexBindingDescriptions = &inputDescr;
                vertInputCreate.vertexAttributeDescriptionCount = 1;
                vertInputCreate.pVertexAttributeDescriptions = &attrDescr;

                //input assembly
                VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreat {};
                inputAssemblyCreat.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                switch (style.renderMode) {
                    case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::SOLID: inputAssemblyCreat.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
                    case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::WIREFRAME: inputAssemblyCreat.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
                    case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::VERTICES: inputAssemblyCreat.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;

                    default: std::unreachable();
                }
                inputAssemblyCreat.primitiveRestartEnable = VK_FALSE;

                //setup dynamic state
                VkPipelineDynamicStateCreateInfo dynamicStateCreate{};
                dynamicStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicStateCreate.pNext = nullptr;
                dynamicStateCreate.dynamicStateCount = sizeof(dynamicStates)/sizeof(*dynamicStates);
                dynamicStateCreate.pDynamicStates = dynamicStates;

                //get the target size
                GLGE::uvec2 extent;
                if (currentTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                    extent = reinterpret_cast<GLGE::Graphic::Window*>(currentTarget.getTarget())->getResolution();
                } else if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                    extent = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize();
                } else 
                {std::unreachable();}
                //viewport state still required
                VkViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float)extent.x;
                viewport.height = (float)extent.y;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                VkRect2D scissor{};
                scissor.offset = {0, 0};
                scissor.extent = {extent.x, extent.y};
                VkPipelineViewportStateCreateInfo viewportStateCreate {};
                viewportStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportStateCreate.viewportCount = 1;
                viewportStateCreate.scissorCount = 1;
                viewportStateCreate.pScissors = &scissor;
                viewportStateCreate.pViewports = &viewport;

                //rasterizer
                VkPipelineRasterizationStateCreateInfo rasterStateCreate {};
                rasterStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterStateCreate.depthClampEnable = VK_FALSE;
                rasterStateCreate.rasterizerDiscardEnable = VK_FALSE;
                rasterStateCreate.polygonMode = VK_POLYGON_MODE_FILL;
                rasterStateCreate.lineWidth = 1.f;
                rasterStateCreate.cullMode = VK_CULL_MODE_NONE;
                rasterStateCreate.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                rasterStateCreate.depthBiasEnable = VK_FALSE;

                //depth stencil state
                VkPipelineDepthStencilStateCreateInfo depthStencilCreate {};
                depthStencilCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depthStencilCreate.depthTestEnable = VK_TRUE;
                depthStencilCreate.depthWriteEnable = false;
                depthStencilCreate.depthCompareOp = VK_COMPARE_OP_ALWAYS;
                depthStencilCreate.depthBoundsTestEnable = VK_FALSE;
                depthStencilCreate.stencilTestEnable = VK_FALSE;
                depthStencilCreate.minDepthBounds = 0.0f;
                depthStencilCreate.maxDepthBounds = 1.0f;

                //multi-sample
                GLGE::i32 sampleCount = 1;
                if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                    sampleCount = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachment(0)->getSamplesPerPixel();
                }
                VkPipelineMultisampleStateCreateInfo multiSampleStateCreate {};
                multiSampleStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multiSampleStateCreate.sampleShadingEnable = (sampleCount > 1) ? VK_TRUE : VK_FALSE;
                multiSampleStateCreate.rasterizationSamples = static_cast<VkSampleCountFlagBits>(sampleCount);

                //color blending
                VkPipelineColorBlendAttachmentState colorBlendAttachment {};
                colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable = VK_FALSE;
                VkPipelineColorBlendStateCreateInfo colorBlendingStateCreate {};
                colorBlendingStateCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                colorBlendingStateCreate.logicOpEnable = VK_FALSE;
                colorBlendingStateCreate.attachmentCount = 1;
                colorBlendingStateCreate.pAttachments = &colorBlendAttachment;

                //setup dynamic rendering
                std::vector<VkFormat> colorAttachmentFormats;
                GLGE::u32 colAttCount = 1;
                if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) 
                {colAttCount = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachmentCount();}
                colorAttachmentFormats.reserve(colAttCount);
                for (size_t i = 0; i < colAttCount; ++i) {
                    if (currentTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                        colorAttachmentFormats.push_back(static_cast<VkFormat>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(reinterpret_cast<GLGE::Graphic::Window*>(currentTarget.getTarget())->getGraphicWindow().get())->getFormat()));
                    } else if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                        colorAttachmentFormats.push_back(static_cast<VkFormat>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachment(i))->getVkFormat()));
                    } else 
                    {std::unreachable();}
                }
                VkPipelineRenderingCreateInfoKHR pipeRenderingCreate {};
                pipeRenderingCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
                pipeRenderingCreate.colorAttachmentCount = colAttCount;
                pipeRenderingCreate.pColorAttachmentFormats = colorAttachmentFormats.data();
                if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                    const auto& fbuff = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend();
                    pipeRenderingCreate.depthAttachmentFormat = fbuff->getDepthAttachmentCount() ? static_cast<VkFormat>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(fbuff->getDepthAttachment(0))->getVkFormat()) : VK_FORMAT_UNDEFINED;
                    pipeRenderingCreate.stencilAttachmentFormat = fbuff->getStencilAttachmentCount() ? static_cast<VkFormat>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(fbuff->getStencilAttachment(0))->getVkFormat()) : VK_FORMAT_UNDEFINED;
                }

                //gather all shader stages for graphics
                std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
                shaderStages.reserve(4);
                for (size_t i = 0; i < vkShader->getModules().size(); ++i) {
                    //get the module
                    const auto& mod = vkShader->getModules()[i];
                    //get reflection information about the state
                    const auto& state = vkShader->getFrontend()->getElements()[i];

                    //get the vulkan stage type
                    VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
                    switch (static_cast<GLGE::Graphic::Shader::Type>(mod.tag))
                    {
                    case GLGE::Graphic::Shader::VERTEX: stage = VK_SHADER_STAGE_VERTEX_BIT; break;
                    case GLGE::Graphic::Shader::TESSELATION_CONTROL: stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
                    case GLGE::Graphic::Shader::TESSELATION_EVALUATION: stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
                    case GLGE::Graphic::Shader::GEOMETRY: stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
                    case GLGE::Graphic::Shader::FRAGMENT: stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                    
                    default:
                        break;
                    }
                    //if the stage is unknown, stop
                    if (stage == VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
                    {continue;}

                    //create the shader stage
                    VkPipelineShaderStageCreateInfo shaderStageCreate {};
                    shaderStageCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    shaderStageCreate.module = reinterpret_cast<VkShaderModule>(mod.shaderModule);
                    shaderStageCreate.stage = stage;
                    shaderStageCreate.pName = state.entryPoint.c_str();
                    shaderStages.push_back(shaderStageCreate);
                }

                //create the actual graphic pipeline
                VkGraphicsPipelineCreateInfo graphicsPipeCreate {};
                graphicsPipeCreate.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                graphicsPipeCreate.pNext = &pipeRenderingCreate;
                graphicsPipeCreate.stageCount = shaderStages.size();
                graphicsPipeCreate.pStages = shaderStages.data();
                graphicsPipeCreate.pVertexInputState = &vertInputCreate;
                graphicsPipeCreate.pInputAssemblyState = &inputAssemblyCreat;
                graphicsPipeCreate.pViewportState = &viewportStateCreate;
                graphicsPipeCreate.pDynamicState = &dynamicStateCreate;
                graphicsPipeCreate.pRasterizationState = &rasterStateCreate;
                graphicsPipeCreate.pMultisampleState = &multiSampleStateCreate;
                graphicsPipeCreate.pColorBlendState = &colorBlendingStateCreate;
                graphicsPipeCreate.pDepthStencilState = (pipeRenderingCreate.depthAttachmentFormat != VK_FORMAT_UNDEFINED) ? &depthStencilCreate : nullptr;
                graphicsPipeCreate.layout = vkLayout;
                graphicsPipeCreate.renderPass = VK_NULL_HANDLE;
                graphicsPipeCreate.subpass = 0;
                graphicsPipeCreate.basePipelineHandle = VK_NULL_HANDLE;
                VkPipeline pipe;
                if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipeCreate, nullptr, &pipe) != VK_SUCCESS)
                {throw GLGE::Exception("Failed to create a graphics pipeline", "GLGE::Graphic::Backend::Graphic::Vulkan::Translators::drawDebug");}

                //store the pipeline and layout
                persistent->pipelines[key] = Persistent::PipelineStorage {
                    .pipeline = pipe,
                    .layout = vkLayout,
                    .generation = currentGen
                };

            }
        }
    }

    //upload the tracked data
    context->getTargetInfoBuffer()->write(extents.data(), extents.size() * sizeof(*extents.data()), 0);
    context->getCameraBuffer()->write(camMatrices.data(), camMatrices.size() * sizeof(*camMatrices.data()), 0);

    //make sure to clean up pipelines that were unused long enough
    constexpr GLGE::u8 pipelineTimeout = 16;
    for (auto it = persistent->pipelines.begin(); it != persistent->pipelines.end(); ++it) {
        //compute generation delta
        //Warning: Generations may wrap around
        GLGE::u8 delta = currentGen - it->second.generation;

        //check if the element is outdated
        if (delta >= pipelineTimeout) {
            //clean up
            vkDestroyPipelineLayout(device, it->second.layout, nullptr);
            vkDestroyPipeline(device, it->second.pipeline, nullptr);
            it = persistent->pipelines.erase(it);
        }
    }
    //make sure to update the generation
    persistent->generation = currentGen;

    //iterate over all buffers
    for (size_t i = 0; i < static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cmdBuff)->getBufferCount(); ++i) {
        //get the command buffer
        VkCommandBuffer cb = reinterpret_cast<VkCommandBuffer>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cmdBuff)->getBuffer(i));

        //initialization
        //bind the index buffer
        VkBuffer ibo = reinterpret_cast<VkBuffer>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Buffer*>(context->getIBO()->getBackendReference().get())->getBuffer());
        vkCmdBindIndexBuffer(cb, ibo, 0, VK_INDEX_TYPE_UINT32);
        //iterate over all streams and bind the vertex buffers
        VkBuffer vbo = reinterpret_cast<VkBuffer>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Buffer*>(context->getVBO()->getBackendReference().get())->getBuffer());
        VkDeviceSize offs = 0;
        vkCmdBindVertexBuffers(cb, i, 1, &vbo, &offs);

        //keep track of the current state
        currentShader = nullptr;
        currentTarget = static_cast<GLGE::Graphic::Window*>(nullptr);
        GLGE::u32 drawIdx = 0;
        GLGE::Graphic::RenderTarget currentActiveTarget = static_cast<GLGE::Graphic::Window*>(nullptr);
        //store the current pipeline
        Persistent::PipelineStorage currentPipe;

        //iterate over all commands
        for (const auto& cmd : context->getCommands()) {
            if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetCamera>(cmd.command)) {
                /*just ignore it*/
            } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetShader>(cmd.command)) {
                //store the current shader
                currentShader = std::get<GLGE::Graphic::DebugContext::Command::SetShader>(cmd.command).shader;
            } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetTarget>(cmd.command)) {
                //store the current target
                currentTarget = std::get<GLGE::Graphic::DebugContext::Command::SetTarget>(cmd.command).target;
            } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::Draw>(cmd.command)) {
                //get the draw cmd
                const auto& drawCmd = std::get<GLGE::Graphic::DebugContext::Command::Draw>(cmd.command);
                //get the current draw extent
                GLGE::uvec2 extent;
                if (currentTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                    extent = reinterpret_cast<GLGE::Graphic::Window*>(currentTarget.getTarget())->getResolution();
                } else if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                    extent = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize();
                } else 
                {std::unreachable();}

                //iterate over all sub-commands
                for (size_t i = drawCmd.cmdStartIdx; i < (drawCmd.cmdStartIdx + drawCmd.drawCount); ++i) {
                    //create a pipeline key to check if the required pipeline exists
                    const auto& subCmd = context->getDrawSubCommands()[i];
                    auto style = subCmd.style;
                    Persistent::PipelineKey key {
                        .shader = currentShader,
                        .target = currentTarget,
                        .blendMode = style.blendMode,
                        .renderMode = style.renderMode
                    };

                    //get the pipeline
                    const auto& pipe = persistent->pipelines.at(key);

                    //check if a pipeline switch is required
                    if (currentPipe != pipe) {
                        //check if the currently active target changed
                        if (currentActiveTarget != currentTarget) {
                            //new dynamic render pass must start

                            //but if an old target is active, the old one must finish
                            if (currentActiveTarget.getTarget()) 
                            {reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkInst->getCommands().pfn_vkCmdEndRenderingKHR)(cb);}

                            //start dynamic rendering
                            std::vector<VkRenderingAttachmentInfoKHR> colorAttachments;
                            GLGE::u32 colAttCount = 1;
                            if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) 
                            {colAttCount = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachmentCount();}
                            colorAttachments.reserve(colAttCount);
                            for (size_t i = 0; i < colAttCount; ++i) {
                                VkRenderingAttachmentInfoKHR colorAttach {};
                                colorAttach.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                                colorAttach.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                                if (currentTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                                    colorAttach.imageView = static_cast<VkImageView>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Window*>(reinterpret_cast<GLGE::Graphic::Window*>(currentTarget.getTarget())->getGraphicWindow().get())->getImageViews()[i]);
                                } else if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                                    colorAttach.imageView = static_cast<VkImageView>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend()->getColorAttachment(i))->getView());
                                } else 
                                {std::unreachable();}
                                colorAttach.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                                colorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                                colorAttachments.push_back(colorAttach);
                            }
                            
                            VkRenderingAttachmentInfoKHR depthAttach {};
                                if (currentTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                                    auto* vkFbuff = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(currentTarget.getTarget())->getBackend().get());
                                    auto* depthAtt = vkFbuff->getDepthAttachmentCount() ? static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Image*>(vkFbuff->getDepthAttachment(0)) : nullptr;
                                    if (depthAtt) {
                                        depthAttach.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                                        depthAttach.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                                        depthAttach.imageView = static_cast<VkImageView>(depthAtt->getView());
                                        depthAttach.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                                        depthAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                                    }
                            }

                            //prepare the rendering
                            VkMemoryBarrier barrierInit {};
                            barrierInit.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
                            barrierInit.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                            barrierInit.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                            vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 1, &barrierInit, 0, nullptr, 0, nullptr);

                            VkRenderingInfoKHR renInfo {};
                            renInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
                            renInfo.colorAttachmentCount = colorAttachments.size();
                            renInfo.pColorAttachments = colorAttachments.data();
                            renInfo.pDepthAttachment = (depthAttach.sType == VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR) ? &depthAttach : nullptr;
                            renInfo.renderArea.offset = {0,0};
                            renInfo.renderArea.extent = {extent.x, extent.y};
                            renInfo.layerCount = 1;
                            renInfo.viewMask = 0;
                            (*reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkInst->getCommands().pfn_vkCmdBeginRenderingKHR))(cb, &renInfo);
        
                            //store new active target
                            currentActiveTarget = currentTarget;
                        }

                        //extract the resource sets
                        std::vector<VkDescriptorSet> sets;
                        sets.reserve(currentShader->getSetCount());
                        for (size_t i = 0; i < currentShader->getSetCount(); ++i) {
                            if (!currentShader->hasSet(i)) {continue;}
                            auto* vkResourceSet = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(currentShader->getResources(0)->getBackend().get());
                            sets.push_back(reinterpret_cast<VkDescriptorSet>(vkResourceSet->getDescriptorSet()));
                        }
                        //bind descriptor sets
                        vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe.layout, 0, sets.size(), sets.data(), 0, nullptr);
                        //start the pipeline
                        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe.pipeline);

                        //set the target-dependent dynamic states
                        VkViewport viewport {};
                        viewport.x = 0;
                        viewport.y = 0;
                        viewport.minDepth = 0.f;
                        viewport.maxDepth = 1.f;
                        viewport.width = extent.x;
                        viewport.height = extent.y;
                        vkCmdSetViewport(cb, 0, 1, &viewport);
                        VkRect2D scissor {};
                        scissor.offset = {0,0};
                        scissor.extent = {extent.x, extent.y};
                        vkCmdSetScissor(cb, 0, 1, &scissor);
                    }

                    //set draw-dependent dynamic states
                    reinterpret_cast<PFN_vkCmdSetDepthWriteEnableEXT>(vkInst->getCommands().pfn_vkCmdSetDepthWriteEnableEXT)(cb, style.depthWrite ? VK_TRUE : VK_FALSE);
                    //set the depth test function
                    reinterpret_cast<PFN_vkCmdSetDepthCompareOpEXT>(vkInst->getCommands().pfn_vkCmdSetDepthCompareOpEXT)(cb, __toVkCompareOp(style.depthTest));
                    //set the cull operation
                    reinterpret_cast<PFN_vkCmdSetCullModeEXT>(vkInst->getCommands().pfn_vkCmdSetCullModeEXT)(cb, __toVkCullMode(style.cullMode));

                    //draw command depends on render mode
                    switch (style.renderMode) {
                        case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::SOLID: 
                            vkCmdDrawIndexed(cb, subCmd.indexCount, 1, subCmd.indexStart, subCmd.pointStart, drawIdx);
                            break;
                        case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::WIREFRAME: 
                            vkCmdDrawIndexed(cb, subCmd.indexCount, 1, subCmd.indexStart, subCmd.pointStart, drawIdx);
                            break;
                        case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::VERTICES: 
                            vkCmdDraw(cb, subCmd.pointCount, 1, subCmd.pointStart, drawIdx);
                            break;

                        default: std::unreachable();
                    }

                    //step the draw command
                    ++drawIdx;
                }
            } else 
            {std::unreachable();}
        }

        //at the end: check if a rendering command must be finished
        if (currentActiveTarget.getTarget()) {
            //end the rendering and wait for it to finish
            reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkInst->getCommands().pfn_vkCmdEndRenderingKHR)(cb);

            VkMemoryBarrier barrierInit {};
            barrierInit.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            barrierInit.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            barrierInit.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 1, &barrierInit, 0, nullptr, 0, nullptr);
        }
    }

    //success
    return true;
}

}