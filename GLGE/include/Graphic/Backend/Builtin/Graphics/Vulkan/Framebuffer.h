/**
 * @file Framebuffer.h
 * @author DM8AT
 * @brief define the vulkan backend implementation of a framebuffer
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_FRAMEBUFFER_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_FRAMEBUFFER_

//add the framebuffer API
#include "Graphic/Backend/Graphics/Framebuffer.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief overload the framebuffer class for Vulkan
     */
    class Framebuffer : public GLGE::Graphic::Backend::Graphic::Framebuffer {
    public:

        /**
         * @brief Construct a new Framebuffer
         * 
         * @warning if a hard cap of the attachment count limit is surpassed this function will throw an exception in debug or clip in release
         * 
         * @param colorAttachmentCount the amount of color attachments to add
         * @param colorAttachments the color attachments to add
         * @param depthAttachmentCount the amount of depth / depth stencil attachments to add
         * @param depthAttachments the depth / depth stencil attachments to add
         * @param stencilAttachmentCount the amount of stencil attachments to add
         * @param stencilAttachments the stencil attachments to add
         * @param combinedDepthStencil `true` if depth and stencil buffer are combined, `false` if they are different
         * @param instance a pointer to the owning backend instance
         */
        Framebuffer(u8 colorAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                    u8 depthAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                    u8 stencilAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                    bool combinedDepthStencil, GLGE::Graphic::Backend::Graphic::Instance* instance);

        /**
         * @brief Destroy the Framebuffer
         */
        virtual ~Framebuffer();

        /**
         * @brief a function used to clear the framebuffer
         * 
         * @param color the color to clear to
         * @param depth the depth to clear to
         * @param stencil the stencil value to clear to
         */
        virtual void onClear(vec4 color, float depth, u32 stencil) override;

        /**
         * @brief a function to call if an image was updated
         * 
         * @param image a pointer to the updated image
         */
        virtual void onImageUpdated(GLGE::Graphic::Backend::Graphic::Image* image) override;

        /**
         * @brief Get the Render Pass
         * 
         * @return `void*` the vulkan render pass
         */
        inline void* getRenderPass() const noexcept
        {return m_renderPass;}

        /**
         * @brief Get the Framebuffer
         * 
         * @return `void*` the vulkan framebuffer
         */
        inline void* getFramebuffer() const noexcept
        {return m_vkFramebuffer;}

    protected:

        /**
         * @brief Create a Render Pass
         */
        void createRenderPass();

        /**
         * @brief Destroy a render pass
         */
        void destroyRenderPass();

        /**
         * @brief Create the Framebuffers
         */
        void createFramebuffers();

        /**
         * @brief destroy the framebuffers
         */
        void destroyFramebuffers();

        /**
         * @brief store the vulkan framebuffer
         */
        void* m_vkFramebuffer = nullptr;
        /**
         * @brief store the render pass
         */
        void* m_renderPass = nullptr;

    };

}

#endif