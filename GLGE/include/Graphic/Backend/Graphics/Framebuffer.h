/**
 * @file Framebuffer.h
 * @author DM8AT
 * @brief define the backend API for a framebuffer
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_FRAMEBUFFER_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_FRAMEBUFFER_

//add exceptions
#include "Core/Exception.h"
//add references
#include "Core/Reference.h"
//images are required for framebuffers
#include "Image.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief an API for a framebuffer backend implementation
     */
    class Framebuffer : public Referable {
    public:

        /**
         * @brief store the maximum color attachment count
         */
        inline static constexpr u8 MAX_COLOR_ATTACHMENTS = 8;
        /**
         * @brief store the maximum depth attachment count
         */
        inline static constexpr u8 MAX_DEPTH_ATTACHMENTS = 1;
        /**
         * @brief store the maximum stencil attachment count
         */
        inline static constexpr u8 MAX_STENCIL_ATTACHMENTS = 1;

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
         * @brief resize all underlying images
         * 
         * @param size the size to switch the images to
         */
        virtual void resize(const uvec2& size) final;

        /**
         * @brief a function used to clear the framebuffer
         * 
         * @param color the color to clear to
         * @param depth the depth to clear to
         * @param stencil the stencil value to clear to
         */
        virtual void onClear(vec4 color, float depth, u32 stencil) = 0;

        /**
         * @brief Get the amount of bound color attachments
         * 
         * @return `u8` the amount of bound color attachments
         */
        inline u8 getColorAttachmentCount() const noexcept
        {return m_colorAttachmentCount;}

        /**
         * @brief Get a specific color attachment
         * 
         * @param index the index of the color attachment to get
         * @return `GLGE::Graphic::Backend::Graphic::Image*` a pointer to the backend image
         */
        inline GLGE::Graphic::Backend::Graphic::Image* getColorAttachment(u8 index) noexcept
        {return m_attachments[COLOR_IDX_OFFSET + index].get();}

        /**
         * @brief Get the amount of bound depth / depth stencil attachments
         * 
         * @return `u8` the amount of bound depth / depth stencil attachments
         */
        inline u8 getDepthAttachmentCount() const noexcept
        {return m_depthAttachmentCount;}

        /**
         * @brief Get a specific depth / depth stencil attachment
         * 
         * @param index the index of the depth attachment to get
         * @return `GLGE::Graphic::Backend::Graphic::Image*` a pointer to the backend image
         */
        inline GLGE::Graphic::Backend::Graphic::Image* getDepthAttachment(u8 index) noexcept
        {return m_attachments[DEPTH_IDX_OFFSET + index].get();}

        /**
         * @brief Get the amount of bound stencil attachments
         * 
         * @return `u8` the amount of bound stencil attachments
         */
        inline u8 getStencilAttachmentCount() const noexcept
        {return m_stencilAttachmentCount;}

        /**
         * @brief Get a specific stencil attachment
         * 
         * @param index the index of the stencil attachment to get
         * @return `GLGE::Graphic::Backend::Graphic::Image*` a pointer to the backend image
         */
        inline GLGE::Graphic::Backend::Graphic::Image* getStencilAttachment(u8 index) noexcept
        {return m_attachments[STENCIL_IDX_OFFSET + index].get();}

        /**
         * @brief get if the framebuffer has a combined depth and stencil buffer
         * 
         * @return `true` if depth and stencil buffer are combined, `false` if they are separated
         */
        inline bool usesDepthStencil() const noexcept
        {return m_hasDepthStencil;}

        /**
         * @brief a function to call if an image was updated
         * 
         * @param image a pointer to the updated image
         */
        virtual void onImageUpdated(GLGE::Graphic::Backend::Graphic::Image* image) = 0;

    protected:

        /**
         * @brief a function used as a hook for resizing
         * 
         * @param size the size the framebuffer was resized to
         */
        virtual void onResize(const uvec2& size) {}

        /**
         * @brief store a combined maximum for all attachments
         */
        inline static constexpr u8 MAX_TOTAL_ATTACHMENTS = MAX_COLOR_ATTACHMENTS + MAX_DEPTH_ATTACHMENTS + MAX_STENCIL_ATTACHMENTS;

        /**
         * @brief store the index offset for color attachments
         */
        inline static constexpr u8 COLOR_IDX_OFFSET = 0;
        /**
         * @brief store the index offset for depth attachments (depth stencil if used)
         */
        inline static constexpr u8 DEPTH_IDX_OFFSET = MAX_COLOR_ATTACHMENTS;
        /**
         * @brief store the index offset for stencil attachments (ignored if depth stencil is used)
         */
        inline static constexpr u8 STENCIL_IDX_OFFSET = MAX_COLOR_ATTACHMENTS + MAX_DEPTH_ATTACHMENTS;

        /**
         * @brief store pointers to all images
         */
        Reference<GLGE::Graphic::Backend::Graphic::Image> m_attachments[MAX_TOTAL_ATTACHMENTS]{};

        /**
         * @brief store the amount of attached color attachments
         */
        u8 m_colorAttachmentCount = 0;
        /**
         * @brief store the amount of attached depth / depth stencil attachments
         */
        u8 m_depthAttachmentCount = 0;
        /**
         * @brief store the amount of attached stencil attachments
         */
        u8 m_stencilAttachmentCount = 0;

        /**
         * @brief `true` if depth and stencil are combined, `false` if they are separated or one of them is only used
         */
        bool m_hasDepthStencil = false;

    };

}

#endif