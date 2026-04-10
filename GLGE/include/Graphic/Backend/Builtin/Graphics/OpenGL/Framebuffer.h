/**
 * @file Framebuffer.h
 * @author DM8AT
 * @brief define the OpenGL implementation for a framebuffer
 * @version 0.1
 * @date 2026-03-13
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
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief overload the framebuffer class for OpenGL
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
         */
        Framebuffer(u8 colorAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                    u8 depthAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                    u8 stencilAttachmentCount, Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                    bool combinedDepthStencil);

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
         * @brief Get the Handle of the framebuffer
         * 
         * @return `u32` the OpenGL handle
         */
        inline u32 getHandle() const noexcept
        {return m_handle;}

        /**
         * @brief a function to call if an image was updated
         * 
         * @param image a pointer to the updated image
         */
        virtual void onImageUpdated(GLGE::Graphic::Backend::Graphic::Image* image) override;

    protected:

        /**
         * @brief store a counter for name generation
         */
        inline static u32 ms_nameCounter = 0;

        /**
         * @brief store the OpenGL handle
         */
        u32 m_handle = 0;
        /**
         * @brief store the name enumerator
         */
        u32 m_name = ++ms_nameCounter;

    };

}

#endif