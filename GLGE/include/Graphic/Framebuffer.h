/**
 * @file Framebuffer.h
 * @author DM8AT
 * @brief A framebuffer is a collection of images that can be rendered to
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_FRAMEBUFFER_
#define _GLGE_GRAPHIC_FRAMEBUFFER_

//include common stuff
#include "Core/Common.h"
//add the base class
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//add images
#include "Image.h"
//add the framebuffer backend
#include "Backend/Graphics/Framebuffer.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store a collection of images that can be rendered to
     */
    class Framebuffer : public BaseClass {
    public:

        /**
         * @brief Construct a new Framebuffer
         * 
         * @param colorAttachments pointer to images that store the color attachments
         * @param depthAttachments pointer to images that store the depth attachments
         * @param stencilAttachments pointer to images that store the stencil attachments
         */
        Framebuffer(std::initializer_list<GLGE::Graphic::Image*> colorAttachments, 
                    std::initializer_list<GLGE::Graphic::Image*> depthAttachments = {}, 
                    std::initializer_list<GLGE::Graphic::Image*> stencilAttachments = {})
        : BaseClass()
        {
            //store all the attachment backends in buffers
            Reference<GLGE::Graphic::Backend::Graphic::Image> _colorAttachments[GLGE::Graphic::Backend::Graphic::Framebuffer::MAX_COLOR_ATTACHMENTS]{};
            Reference<GLGE::Graphic::Backend::Graphic::Image> _depthAttachments[GLGE::Graphic::Backend::Graphic::Framebuffer::MAX_DEPTH_ATTACHMENTS]{};
            Reference<GLGE::Graphic::Backend::Graphic::Image> _stencilAttachments[GLGE::Graphic::Backend::Graphic::Framebuffer::MAX_STENCIL_ATTACHMENTS]{};

            for (size_t i = 0; i < colorAttachments.size(); ++i)
            {_colorAttachments[i] = colorAttachments.begin()[i]->backend();}
            for (size_t i = 0; i < depthAttachments.size(); ++i)
            {_depthAttachments[i] = depthAttachments.begin()[i]->backend();}
            for (size_t i = 0; i < stencilAttachments.size(); ++i)
            {_stencilAttachments[i] = stencilAttachments.begin()[i]->backend();}

            //check for a depth stencil combination
            bool depthStencil = false;
            if (depthAttachments.size())
            {depthStencil = _depthAttachments[0]->getFormat().order == GLGE::Graphic::PixelFormat::Order::DEPTH_STENCIL;}

            //create the framebuffer
            m_fbuff = getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createFramebuffer(
                colorAttachments.size(), _colorAttachments, depthAttachments.size(), _depthAttachments, stencilAttachments.size(), _stencilAttachments, depthStencil,
                getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()
            );
        }

        /**
         * @brief resize the color buffer
         * 
         * @param size the size to change to
         */
        inline void resize(const uvec2& size) 
        {m_fbuff->resize(size);}

        /**
         * @brief Get the Backend framebuffer
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Framebuffer>` a pointer to the backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> getBackend() noexcept
        {return m_fbuff.get();}

    protected:

        /**
         * @brief store the framebuffer instance
         */
        Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> m_fbuff;

    };

}

#endif